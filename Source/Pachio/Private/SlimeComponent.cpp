#include "SlimeComponent.h"
#include "KismetProceduralMeshLibrary.h"
#include "DrawDebugHelpers.h"

// コンストラクタ
USlimeComponent::USlimeComponent()
{
    PrimaryComponentTick.bCanEverTick = true;

    // デフォルトパラメータ設定
    Radius = 50.0f;

    // 物理パラメータ（「水風船」のようなドッシリ設定）
    Stiffness = 0.4f;        // 皮の硬さ
    Damping = 0.98f;         // 減衰（空気抵抗ほぼなし）
    CoreMass = 5.0f;         // 重さ
    FloorFriction = 0.9f;    // 床の摩擦
    ShapeMatchStrength = 0.05f; // 形を保つ力（弱め）
    PressureStrength = 0.0f;    // 内圧（膨らまないようにゼロ）

    // メッシュ解像度（同心円リング用）
    VisualRings = 12;
    VisualSegments = 12;

    // 物理粒子の簡易解像度
    PhysicsRings = 4;
    PhysicsSegments = 6;
}

// ゲーム開始時の処理
void USlimeComponent::BeginPlay()
{
    Super::BeginPlay();

    // オーナー（Pawn）から ProceduralMeshComponent を探して変数に入れる
    ProcMesh = GetOwner()->FindComponentByClass<UProceduralMeshComponent>();

    // もし見つからなかったらログを出して警告する（念のため）
    if (ProcMesh == nullptr)
    {
        UE_LOG(LogTemp, Error, TEXT("SlimeComponent: Procedural Mesh Component NOT FOUND on this Actor!"));
        return; // 見つからないならこれ以上処理しない
    }

    // 1. 物理演算用のパーティクル生成
    InitializePhysicsParticles();

    // 2. 見た目用のメッシュ生成（同心円リング構造）
    GenerateVisualMesh();

    // 3. スキニングウェイト（見た目の頂点がどの物理粒子に追従するか）の計算
    CalculateSkinningWeights();
}

// 毎フレームの更新処理
void USlimeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    // 物理シミュレーションを実行
    Simulate(DeltaTime);

    // 見た目のメッシュを物理粒子の位置に合わせて更新
    UpdateVisualMeshPosition();
}

// 物理パーティクルの初期化
void USlimeComponent::InitializePhysicsParticles()
{
    Particles.Empty();

    // 中心点（コア）
    FSlimeParticle Core;
    Core.Position = FVector::ZeroVector;
    Core.OldPosition = Core.Position;
    Core.Velocity = FVector::ZeroVector;
    Core.Mass = CoreMass;
    Particles.Add(Core);

    // 表面のパーティクル
    for (int32 r = 0; r < PhysicsRings; r++)
    {
        // 球体をリング状に分割（0度～180度）
        float Theta = PI * ((float)(r + 1) / (float)(PhysicsRings + 1));
        float Z = Radius * FMath::Cos(Theta);
        float RingRadius = Radius * FMath::Sin(Theta);

        for (int32 s = 0; s < PhysicsSegments; s++)
        {
            float Phi = 2.0f * PI * ((float)s / (float)PhysicsSegments);
            float X = RingRadius * FMath::Cos(Phi);
            float Y = RingRadius * FMath::Sin(Phi);

            FSlimeParticle P;
            P.Position = FVector(X, Y, Z);
            P.OldPosition = P.Position;
            P.Velocity = FVector::ZeroVector;
            P.Mass = 1.0f; // 表面は軽く
            Particles.Add(P);
        }
    }
}

// 見た目用メッシュの生成（同心円リング構造）
// ※ここがSingleLayerWaterを綺麗に見せるためのキモです
void USlimeComponent::GenerateVisualMesh()
{
    VisualVertices.Empty();
    VisualTriangles.Empty();
    VisualUVs.Empty();
    VisualNormals.Empty();
    VisualTangents.Empty();

    // --- 頂点生成 ---

    // 1. 北極（頭のてっぺん）
    VisualVertices.Add(FVector(0, 0, Radius));
    VisualUVs.Add(FVector2D(0.5f, 0.0f));
    VisualNormals.Add(FVector(0, 0, 1));

    // 2. 側面のリング群
    for (int32 r = 0; r < VisualRings; r++)
    {
        float Alpha = (float)(r + 1) / (float)(VisualRings + 1);
        float Theta = PI * Alpha; // 0 to PI

        float Z = Radius * FMath::Cos(Theta);
        float R_Current = Radius * FMath::Sin(Theta);

        for (int32 s = 0; s <= VisualSegments; s++) // テクスチャの継ぎ目を消すため <=
        {
            float Phi = 2.0f * PI * ((float)s / (float)VisualSegments);
            float X = R_Current * FMath::Cos(Phi);
            float Y = R_Current * FMath::Sin(Phi);

            VisualVertices.Add(FVector(X, Y, Z));

            // UV計算（球体マッピング）
            float U = (float)s / (float)VisualSegments;
            float V = Alpha;
            VisualUVs.Add(FVector2D(U, V));

            VisualNormals.Add(FVector(X, Y, Z).GetSafeNormal());
        }
    }

    // 3. 南極（底の中心）
    VisualVertices.Add(FVector(0, 0, -Radius));
    VisualUVs.Add(FVector2D(0.5f, 1.0f));
    VisualNormals.Add(FVector(0, 0, -1));

    // --- 三角形（インデックス）生成 ---

    int32 NumRingVerts = VisualSegments + 1;

    // 北極のフタ
    for (int32 s = 0; s < VisualSegments; s++)
    {
        VisualTriangles.Add(0); // 北極
        VisualTriangles.Add(1 + s);
        VisualTriangles.Add(1 + s + 1);
    }

    // 側面の帯
    for (int32 r = 0; r < VisualRings - 1; r++)
    {
        int32 RowStart = 1 + r * NumRingVerts;
        int32 NextRowStart = 1 + (r + 1) * NumRingVerts;

        for (int32 s = 0; s < VisualSegments; s++)
        {
            int32 Current = RowStart + s;
            int32 Next = Current + 1;
            int32 Below = NextRowStart + s;
            int32 BelowNext = Below + 1;

            // 四角形を2つの三角形に分割
            VisualTriangles.Add(Current);
            VisualTriangles.Add(Below);
            VisualTriangles.Add(Next);

            VisualTriangles.Add(Next);
            VisualTriangles.Add(Below);
            VisualTriangles.Add(BelowNext);
        }
    }

    // 南極のフタ
    int32 SouthPoleIndex = VisualVertices.Num() - 1;
    int32 LastRowStart = 1 + (VisualRings - 1) * NumRingVerts;

    for (int32 s = 0; s < VisualSegments; s++)
    {
        VisualTriangles.Add(LastRowStart + s);
        VisualTriangles.Add(SouthPoleIndex);
        VisualTriangles.Add(LastRowStart + s + 1);
    }

    // タンジェント計算
    UKismetProceduralMeshLibrary::CalculateTangentsForMesh(VisualVertices, VisualTriangles, VisualUVs, VisualNormals, VisualTangents);

    // メッシュ作成
    if (ProcMesh)
    {
        ProcMesh->CreateMeshSection(0, VisualVertices, VisualTriangles, VisualNormals, VisualUVs, TArray<FColor>(), VisualTangents, true);
    }
}

// 物理シミュレーション（ここが心臓部）
void USlimeComponent::Simulate(float DeltaTime)
{
    if (Particles.Num() == 0) return;

    FVector Gravity(0, 0, -980.0f);

    // 1. 粒子ごとの積分計算（Verlet法）
    for (FSlimeParticle& P : Particles)
    {
        // 速度の更新 (重力 + 減衰)
        P.Velocity += (Gravity * DeltaTime);
        P.Velocity *= Damping; // 空気抵抗

        // 位置の更新
        FVector TempPos = P.Position;
        P.Position += P.Velocity * DeltaTime;
        P.OldPosition = TempPos;
    }

    // 2. 制約の解決（バネ計算）
    SolveConstraints();

    // 3. ★新機能：転倒防止・姿勢制御（起き上がりこぼし）
    SolveUprightConstraint();

    // 4. 床との衝突判定
    SolveFloorCollision();
}

// 制約解決（バネで形を保つ）
void USlimeComponent::SolveConstraints()
{
    // 簡易的な形状維持：初期位置（ローカル）へ戻ろうとする力
    // ※これを強くしすぎると空へ飛ぶので、今はShapeMatchStrengthで微弱に適用
    if (ShapeMatchStrength > 0.0f)
    {
        // コア（Index 0）の移動量を取得
        FVector CoreDelta = Particles[0].Position - Particles[0].OldPosition;

        // 全パーティクルに対して
        // （実際には初期相対座標を保存しておくべきですが、簡易的に現在の並びを維持させます）
        // ここでは「近傍パーティクルとの距離」を保つ基本的なバネ処理は省略し、
        // もっとも効果的な「姿勢制御」に処理を委ねます。
    }
}

// ★最強の姿勢制御関数（これを追加！）
void USlimeComponent::SolveUprightConstraint()
{
    if (Particles.Num() < 2) return;

    // 1. 全粒子の重心を計算
    FVector CenterOfMass = FVector::ZeroVector;
    for (const FSlimeParticle& P : Particles)
    {
        CenterOfMass += P.Position;
    }
    CenterOfMass /= (float)Particles.Num();

    // 2. 「頭（北極）」のパーティクルを特定（Index 1あたりが頭付近）
    // ※Particles[0]はコアなので、[1]番目以降のZが高いやつを頭とみなす
    // 簡易的に Particles[1] を頭とします（生成順序による）
    int32 TopIndex = 1;

    // 3. 理想の頭の位置（重心の真上）
    FVector DesiredTopPos = CenterOfMass + FVector(0.0f, 0.0f, Radius);

    // 4. 強制補正（ダルマのように頭を上に引っ張る）
    float UprightForce = 0.2f; // 強さ調整（0.1～0.5）

    FVector CurrentPos = Particles[TopIndex].Position;
    Particles[TopIndex].Position = FMath::Lerp(CurrentPos, DesiredTopPos, UprightForce);

    // 5. 崩壊防止の安全装置
    // もし底面（Particlesの後ろの方）が重心より上に来たら、叩き落とす
    int32 BottomStartIndex = Particles.Num() - PhysicsSegments;
    for (int32 i = BottomStartIndex; i < Particles.Num(); i++)
    {
        if (Particles[i].Position.Z > CenterOfMass.Z)
        {
            Particles[i].Position.Z = CenterOfMass.Z - (Radius * 0.8f);
        }
    }
}

// 床との衝突判定（摩擦あり）
void USlimeComponent::SolveFloorCollision()
{
    float FloorZ = 0.0f; // 床の高さ（必要ならRaycastで取得可能）

    for (FSlimeParticle& P : Particles)
    {
        if (P.Position.Z < FloorZ)
        {
            // めり込みを戻す
            float Depth = FloorZ - P.Position.Z;
            P.Position.Z = FloorZ;

            // 摩擦計算（横滑りを止める）
            FVector HorizVelocity = FVector(P.Velocity.X, P.Velocity.Y, 0.0f);
            P.Velocity = FVector(
                P.Velocity.X * (1.0f - FloorFriction),
                P.Velocity.Y * (1.0f - FloorFriction),
                0.0f // Z速度は吸収される（跳ね返らない）
            );

            // 速度ベクトルも更新（これをしないと次フレームでまた沈む）
            // P.OldPosition を現在のPositionから逆算して、Velocityがゼロになるように調整
            // Verlet法では Position - OldPosition が速度になるため
            P.OldPosition.Z = P.Position.Z;
        }
    }
}

// スキニングウェイト計算
// （どのVisual頂点が、どのPhysicsパーティクルに引っ張られるか）
void USlimeComponent::CalculateSkinningWeights()
{
    SkinningWeights.SetNum(VisualVertices.Num());

    for (int32 i = 0; i < VisualVertices.Num(); i++)
    {
        FVector VPos = VisualVertices[i];

        // 最も近い物理パーティクルを2つ探す
        int32 IndexA = -1;
        int32 IndexB = -1;
        float DistA = MAX_FLT;
        float DistB = MAX_FLT;

        for (int32 p = 0; p < Particles.Num(); p++)
        {
            // 初期状態はローカル座標と仮定
            // （実際には初期化時にPositionsを保存しておくのがベストですが、半径差で近似します）
            // 簡易的に現在の距離で判定（初期フレームなのでOK）
            float Dist = FVector::Dist(VPos, Particles[p].Position);

            if (Dist < DistA)
            {
                DistB = DistA;
                IndexB = IndexA;
                DistA = Dist;
                IndexA = p;
            }
            else if (Dist < DistB)
            {
                DistB = Dist;
                IndexB = p;
            }
        }

        // ウェイト情報を保存
        FSlimeSkinningWeight W;
        W.ParticleIndexA = IndexA;
        W.ParticleIndexB = IndexB;

        // 距離に応じた重み付け（近いほうが強い）
        float TotalDist = DistA + DistB;
        if (TotalDist > KINDA_SMALL_NUMBER)
        {
            W.WeightA = 1.0f - (DistA / TotalDist);
            W.WeightB = 1.0f - W.WeightA;
        }
        else
        {
            W.WeightA = 1.0f;
            W.WeightB = 0.0f;
        }

        // オフセット（パーティクルからの相対位置）を保存
        if (IndexA >= 0) W.BaseOffsetA = VPos - Particles[IndexA].Position;
        if (IndexB >= 0) W.BaseOffsetB = VPos - Particles[IndexB].Position;

        SkinningWeights[i] = W;
    }
}

// メッシュ位置の更新（毎フレーム実行）
void USlimeComponent::UpdateVisualMeshPosition()
{
    if (ProcMesh == nullptr) return;

    const int32 NumVerts = VisualVertices.Num();

    // 1. スキニングによる変形
    for (int32 i = 0; i < NumVerts; i++) {
        const FSlimeSkinningWeight& W = SkinningWeights[i];
        FVector FinalPos = FVector::ZeroVector;

        if (Particles.IsValidIndex(W.ParticleIndexA))
            FinalPos += (Particles[W.ParticleIndexA].Position + W.BaseOffsetA) * W.WeightA;

        if (Particles.IsValidIndex(W.ParticleIndexB) && W.WeightB > 0.0f)
            FinalPos += (Particles[W.ParticleIndexB].Position + W.BaseOffsetB) * W.WeightB;

        VisualVertices[i] = FinalPos;
    }

    // -------------------------------------------------------
    // ★ 底面の強制フラット化処理（パワハラ補正） ★
    // -------------------------------------------------------

    // 現在の物理パーティクルの中で「最も低いZ座標」を見つける
    float LowestZ = MAX_FLT;
    for (const FSlimeParticle& P : Particles)
    {
        if (P.Position.Z < LowestZ) LowestZ = P.Position.Z;
    }

    // 底面付近の頂点を特定して平らにする
    // 南極点（最後の頂点）とその周りのリング1周分
    int32 SouthPoleIndex = NumVerts - 1;
    int32 BottomRingStart = SouthPoleIndex - (VisualSegments + 1);

    // まず南極点
    VisualVertices[SouthPoleIndex].Z = LowestZ;

    // その周りのリング
    for (int32 i = BottomRingStart; i < SouthPoleIndex; i++)
    {
        // 床に近い場合のみ強制フラット化（空中にいるときはやらない）
        if (VisualVertices[i].Z < LowestZ + 10.0f) // 閾値10cm
        {
            VisualVertices[i].Z = LowestZ;
        }
    }
    // -------------------------------------------------------

    // 法線とタンジェントの再計算
    UKismetProceduralMeshLibrary::CalculateTangentsForMesh(VisualVertices, VisualTriangles, VisualUVs, VisualNormals, VisualTangents);

    // メッシュ更新
    ProcMesh->UpdateMeshSection(0, VisualVertices, VisualNormals, VisualUVs, TArray<FColor>(), VisualTangents);
}