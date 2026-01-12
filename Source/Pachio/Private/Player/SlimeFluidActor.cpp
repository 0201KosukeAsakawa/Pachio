// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/SlimeFluidActor.h"
#include "Kismet/KismetSystemLibrary.h"
#include "DrawDebugHelpers.h"

USlimeFluidComponent::USlimeFluidComponent()
{
    PrimaryComponentTick.bCanEverTick = true;

    Mesh = CreateDefaultSubobject<UProceduralMeshComponent>("SlimeMesh");
    Mesh->SetupAttachment(this);

    Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    Mesh->SetCollisionObjectType(ECC_WorldDynamic);
    Mesh->SetCollisionResponseToAllChannels(ECR_Block);

    Mesh->bUseComplexAsSimpleCollision = true;

    Mesh->SetSimulatePhysics(false);
}


void USlimeFluidComponent::BeginPlay()
{
    Super::BeginPlay();

    GenerateSphere();

    // 初期コア中心を計算
    LocalCoreCenter = FVector::ZeroVector;
    for (const FSlimeVertex& V : Vertices)
    {
        LocalCoreCenter += V.Position;
    }
    LocalCoreCenter /= Vertices.Num();

    // ターゲット平均半径を計算
    TargetAverageRadius = 0.f;
    for (const FSlimeVertex& V : Vertices)
    {
        TargetAverageRadius += V.Position.Size();
    }
    TargetAverageRadius /= Vertices.Num();

    if (SlimeMaterial)
    {
        Mesh->SetMaterial(0, SlimeMaterial);
    }
}

void USlimeFluidComponent::TickComponent(
    float DeltaTime,
    ELevelTick TickType,
    FActorComponentTickFunction* ThisTickFunction
)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    ElapsedTime += DeltaTime;
    if (ElapsedTime < 0.1f)
    {
        return; // 初期安定化
    }

    DetectAllContacts();
    UpdateFluid(DeltaTime);
    DrawDebugVisualization();
}

/* ===============================
   Mesh Generation
================================ */

void USlimeFluidComponent::GenerateSphere()
{
    Vertices.Empty();
    Triangles.Empty();
    InitialVertexPositions.Empty(); // 初期位置をクリア

    TArray<FVector> Positions;
    TArray<FVector> Normals;
    TArray<FVector2D> UVs;

    float CoreRadiusThreshold = Radius * CoreRadiusRatio;

    for (int32 y = 0; y <= Rings; y++)
    {
        float V = (float)y / Rings;
        float Phi = V * PI;

        for (int32 x = 0; x <= Segments; x++)
        {
            float U = (float)x / Segments;
            float Theta = U * PI * 2;

            FVector P(
                Radius * FMath::Sin(Phi) * FMath::Cos(Theta),
                Radius * FMath::Sin(Phi) * FMath::Sin(Theta),
                Radius * FMath::Cos(Phi)
            );

            FVector N = P.GetSafeNormal();
            float DistFromCenter = P.Size();

            FSlimeVertex Vtx;
            Vtx.Position = P;
            Vtx.Velocity = FVector::ZeroVector;
            Vtx.Normal = N;

            // コア判定：中心からの距離がコア半径以下ならコア頂点
            Vtx.bIsCore = (DistFromCenter <= CoreRadiusThreshold);

            // SurfaceWeight: 0(コア中心)～1(表面)
            Vtx.SurfaceWeight = FMath::Clamp(DistFromCenter / Radius, 0.f, 1.f);

            Vertices.Add(Vtx);
            Positions.Add(P);
            Normals.Add(N);
            UVs.Add(FVector2D(U, V));

            // 初期位置を保存
            InitialVertexPositions.Add(P);
        }
    }

    for (int32 y = 0; y < Rings; y++)
    {
        for (int32 x = 0; x < Segments; x++)
        {
            int32 i0 = x + y * (Segments + 1);
            int32 i1 = i0 + 1;
            int32 i2 = i0 + Segments + 1;
            int32 i3 = i2 + 1;

            Triangles.Append({ i0, i2, i1, i1, i2, i3 });
        }
    }

    // 頂点の隣接情報を構築（力の伝播用）
    BuildVertexNeighbors();

    Mesh->CreateMeshSection(
        0,
        Positions,
        Triangles,
        Normals,
        UVs,
        {},
        {},
        true
    );
}

/* ===============================
   Build Vertex Neighbors
================================ */

void USlimeFluidComponent::BuildVertexNeighbors()
{
    VertexNeighbors.Empty();
    VertexNeighbors.SetNum(Vertices.Num());

    // 各三角形から隣接情報を構築
    for (int32 i = 0; i < Triangles.Num(); i += 3)
    {
        int32 v0 = Triangles[i];
        int32 v1 = Triangles[i + 1];
        int32 v2 = Triangles[i + 2];

        // 各頂点に隣接頂点を追加（重複チェック付き）
        auto AddNeighbor = [this](int32 Vertex, int32 Neighbor)
            {
                if (Vertex >= 0 && Vertex < VertexNeighbors.Num())
                {
                    // VertexNeighbors[Vertex] は FVertexNeighborData 型
                    // その中の Neighbors (TArray<int32>) にアクセス
                    if (!VertexNeighbors[Vertex].Neighbors.Contains(Neighbor))
                    {
                        VertexNeighbors[Vertex].Neighbors.Add(Neighbor);
                    }
                }
            };

        // 三角形の各辺を隣接関係として記録
        AddNeighbor(v0, v1);
        AddNeighbor(v0, v2);
        AddNeighbor(v1, v0);
        AddNeighbor(v1, v2);
        AddNeighbor(v2, v0);
        AddNeighbor(v2, v1);
    }
}

/* ===============================
   Contact Detection
================================ */

void USlimeFluidComponent::AddContact(
    const FVector& WorldPos,
    const FVector& WorldNormal,
    float Strength
)
{
    FTransform T = GetComponentTransform();

    FSlimeContact C;
    C.LocalPosition = T.InverseTransformPosition(WorldPos);
    C.Normal = T.InverseTransformVector(WorldNormal).GetSafeNormal();
    C.Strength = Strength;

    // 地面判定：法線が上向き（ワールド座標でZ > 0）
    if (bEnableGroundSpecialHandling)
    {
        float UpDot = FVector::DotProduct(WorldNormal, FVector::UpVector);
        float AngleFromUp = FMath::Acos(UpDot) * 180.0f / PI;
        C.bIsGround = (AngleFromUp <= GroundAngleThreshold);
    }
    else
    {
        C.bIsGround = false;
    }

    Contacts.Add(C);
}

void USlimeFluidComponent::DetectAllContacts()
{
    FVector Center = GetComponentLocation();
    FTransform T = GetComponentTransform();
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(GetOwner());

    if (bUseMeshBasedCollision)
    {
        // メッシュの頂点から接触検出
        for (int32 i = 0; i < Vertices.Num(); i += 3) // 全頂点だと重いので間引く
        {
            const FSlimeVertex& V = Vertices[i];
            FVector WorldPos = T.TransformPosition(V.Position);

            // 頂点から外向きにレイキャスト
            FVector WorldNormal = T.TransformVector(V.Normal);
            FVector Start = WorldPos;
            FVector End = WorldPos + WorldNormal * 20.f;

            FHitResult Hit;
            if (GetWorld()->LineTraceSingleByChannel(
                Hit,
                Start,
                End,
                ECC_WorldStatic,
                Params
            ))
            {
                // 接触点を表面から少し外側に配置（めり込み防止）
                FVector AdjustedContactPos = Hit.ImpactPoint + Hit.ImpactNormal * ContactPenetrationOffset;

                float Distance = (Hit.ImpactPoint - WorldPos).Size();
                float ContactStrength = 300.f / FMath::Max(Distance, 1.0f);

                AddContact(
                    AdjustedContactPos,
                    Hit.ImpactNormal,
                    ContactStrength
                );
            }
        }
    }
    else
    {
        // 従来の固定方向レイキャスト
        TArray<FVector> Directions = {
            FVector(0, 0, -1),
            FVector(1, 0, -0.5), FVector(-1, 0, -0.5),
            FVector(0, 1, -0.5), FVector(0, -1, -0.5),
            FVector(1, 0, 0), FVector(-1, 0, 0),
            FVector(0, 1, 0), FVector(0, -1, 0)
        };

        for (const FVector& Dir : Directions)
        {
            FVector Start = Center;
            FVector End = Center + Dir.GetSafeNormal() * (Radius + 15.f);

            FHitResult Hit;
            if (GetWorld()->LineTraceSingleByChannel(
                Hit,
                Start,
                End,
                ECC_WorldStatic,
                Params
            ))
            {
                // 接触点を表面から少し外側に配置
                FVector AdjustedContactPos = Hit.ImpactPoint + Hit.ImpactNormal * ContactPenetrationOffset;

                float Distance = (Hit.ImpactPoint - Center).Size();
                float Penetration = FMath::Max(0.f, Radius - Distance);
                float ContactStrength = 200.f + Penetration * 50.f;

                AddContact(
                    AdjustedContactPos,
                    Hit.ImpactNormal,
                    ContactStrength
                );
            }
        }
    }
}

/* ===============================
   Fluid Simulation
================================ */

void USlimeFluidComponent::UpdateFluid(float DeltaTime)
{
    /* =====================
       動的コア中心の更新
    ===================== */
    UpdateCoreCenter(DeltaTime);

    /* =====================
       平均半径の計算と誤差
    ===================== */
    float CurrentAverageRadius = 0.f;
    for (const FSlimeVertex& V : Vertices)
    {
        CurrentAverageRadius += V.Position.Size();
    }
    CurrentAverageRadius /= Vertices.Num();

    float RadiusError = TargetAverageRadius - CurrentAverageRadius;

    /* =====================
       地面接触の検出
    ===================== */
    bool bHasGroundContact = false;
    FVector GroundPlanePosition = FVector::ZeroVector;
    FVector GroundPlaneNormal = FVector::UpVector;

    if (bEnableGroundSpecialHandling && bFlattenBottomOnGround)
    {
        for (const FSlimeContact& C : Contacts)
        {
            if (C.bIsGround)
            {
                bHasGroundContact = true;
                GroundPlanePosition = C.LocalPosition;
                GroundPlaneNormal = C.Normal;
                break; // 最初の地面接触を使用
            }
        }
    }

    /* =====================
       頂点ごとのシミュレーション
    ===================== */
    TArray<FVector> Forces;
    Forces.SetNum(Vertices.Num());

    TArray<FVector> NewPositions;
    NewPositions.Reserve(Vertices.Num());

    for (int32 i = 0; i < Vertices.Num(); i++)
    {
        FSlimeVertex& V = Vertices[i];
        FVector Force = FVector::ZeroVector;

        // コア頂点は変形しない（剛体として扱う）
        if (V.bIsCore)
        {
            // コア中心への強い拘束
            Force += (LocalCoreCenter - V.Position) * CoreStiffness;

            // コア頂点の速度減衰（ほぼ静止）
            V.Velocity *= 0.5f;
        }
        else // 表面頂点は変形可能
        {
            float Stiffness = FMath::Lerp(SurfaceStiffness, SurfaceSoftness, V.SurfaceWeight);

            /* コア中心への拘束 */
            if (bPreserveVolume)
            {
                Force += (LocalCoreCenter - V.Position) * Stiffness * RecoverySpeed;
            }
            else
            {
                // 体積保存なしの場合は弱い拘束のみ
                Force += (LocalCoreCenter - V.Position) * Stiffness * 0.3f;
            }

            /* 半径保持 */
            if (bPreserveVolume)
            {
                Force += V.Normal * RadiusError * 0.6f * RecoverySpeed;
            }

            /* 追従ラグ（表面の遅れ効果） */
            Force += -V.Velocity * FollowLagStrength * V.SurfaceWeight;

            /* ノイズ（表面のみ） */
            FVector Noise(
                FMath::PerlinNoise1D(GetWorld()->TimeSeconds * 3.0f + V.Position.X * 0.1f),
                FMath::PerlinNoise1D(GetWorld()->TimeSeconds * 3.0f + V.Position.Y * 0.1f),
                FMath::PerlinNoise1D(GetWorld()->TimeSeconds * 3.0f + V.Position.Z * 0.1f)
            );
            Force += Noise * NoiseStrength * V.SurfaceWeight;

            /* ジグル（ぷるぷる揺れ） */
            FVector JiggleForce = V.Velocity * JiggleAmount * V.SurfaceWeight;
            Force += JiggleForce;

            /* バウンス効果（弾む感じ） */
            if (V.Velocity.Size() > 1.0f)
            {
                FVector BounceForce = V.Velocity.GetSafeNormal() * V.Velocity.SizeSquared() * 0.01f * BounceFactor;
                Force += BounceForce * V.SurfaceWeight;
            }

            /* めり込み防止 */
            if (bPreventPenetration)
            {
                for (const FSlimeContact& C : Contacts)
                {
                    FVector ToV = V.Position - C.LocalPosition;
                    float DistToContact = FVector::DotProduct(ToV, C.Normal);

                    // 接触面より内側（めり込み）の場合
                    if (DistToContact < 0.0f)
                    {
                        // 強い押し戻し力
                        Force += C.Normal * (-DistToContact) * PenetrationResistance;
                    }
                }
            }

            /* 接触による変形（表面のみ） */
            for (const FSlimeContact& C : Contacts)
            {
                FVector ToV = V.Position - C.LocalPosition;
                float Dist = ToV.Size();
                float InfluenceRadius = Radius * ContactInfluenceRadius;

                if (Dist < InfluenceRadius && Dist > KINDA_SMALL_NUMBER)
                {
                    FVector Dir = ToV / Dist;

                    // 接触方向との内積を計算（-1=反対側, 0=横, 1=接触側）
                    float DirectionDot = FVector::DotProduct(Dir, -C.Normal);

                    // 反対側の変形を防ぐ
                    if (bPreventOppositeSideDeformation && DirectionDot < -0.1f)
                    {
                        continue;
                    }

                    // 接触面より外側の頂点のみ変形（めり込み防止）
                    float DistToContactPlane = FVector::DotProduct(ToV, C.Normal);
                    if (bPreventPenetration && DistToContactPlane < 0.0f)
                    {
                        continue; // 接触面より内側の頂点はスキップ
                    }

                    // 接触方向に応じた影響度（接触方向ほど影響大）
                    float DirectionalWeight = FMath::Lerp(
                        1.0f,
                        FMath::Max(0.0f, DirectionDot),
                        ContactDirectionality
                    );

                    float Falloff = 1.f - (Dist / InfluenceRadius);
                    Falloff = FMath::Pow(Falloff, 1.5f);
                    Falloff *= DirectionalWeight; // 方向性を加味

                    // 地面接触時の特殊処理
                    float FinalDeformationSpeed = DeformationSpeed;
                    if (C.bIsGround && bEnableGroundSpecialHandling)
                    {
                        FinalDeformationSpeed *= GroundSquashMultiplier; // 地面でより潰れる
                    }

                    float Compression = C.Strength * Falloff * V.SurfaceWeight * FinalDeformationSpeed;

                    // 押し潰し（接触法線方向のみ）
                    Force += -C.Normal * Compression;

                    // 接触面方向の速度を減衰
                    float VelocityDamping = 0.7f;
                    if (C.bIsGround && bEnableGroundSpecialHandling)
                    {
                        VelocityDamping = 0.9f; // 地面では速度をより減衰（粘着）
                    }
                    V.Velocity -= FVector::DotProduct(V.Velocity, C.Normal) * C.Normal * VelocityDamping;

                    // 横へ広がる（接触面に沿って）
                    FVector Tangent = Dir - C.Normal * FVector::DotProduct(Dir, C.Normal);
                    if (!Tangent.IsNearlyZero())
                    {
                        // 接触側の頂点ほど横に広がる
                        float SpreadWeight = FMath::Max(0.0f, DirectionDot);
                        float FinalSpreadStrength = SpreadStrength;

                        if (C.bIsGround && bEnableGroundSpecialHandling)
                        {
                            FinalSpreadStrength *= 1.3f; // 地面でより広がる
                        }

                        Force += Tangent.GetSafeNormal() * Compression * FinalSpreadStrength * SpreadWeight;
                    }

                    // 地面への粘着力
                    if (C.bIsGround && bEnableGroundSpecialHandling && GroundStickiness > 0.0f)
                    {
                        FVector ToContact = C.LocalPosition - V.Position;
                        Force += ToContact * GroundStickiness * Falloff;
                    }
                }
            }
        }

        /* =====================
           底面の平面化（地面接触時）
        ===================== */
        if (bHasGroundContact && bFlattenBottomOnGround && !V.bIsCore)
        {
            // 頂点が地面に近い（下側の）頂点かチェック
            float DistToGround = FVector::DotProduct(V.Position - GroundPlanePosition, GroundPlaneNormal);

            // 下側の頂点（地面から一定範囲内）を平面化
            if (DistToGround < Radius * BottomFlattenRadius)
            {
                // 頂点を地面の平面に押し付ける
                FVector ProjectedPos = V.Position - GroundPlaneNormal * DistToGround;
                FVector FlattenForce = (ProjectedPos - V.Position) * BottomFlattenStrength;

                // 地面に近いほど強く平面化
                float FlattenWeight = 1.0f - (DistToGround / (Radius * BottomFlattenRadius));
                FlattenWeight = FMath::Clamp(FlattenWeight, 0.0f, 1.0f);
                FlattenWeight = FMath::Pow(FlattenWeight, 2.0f); // 非線形な減衰

                Force += FlattenForce * FlattenWeight * V.SurfaceWeight;
            }
        }

        // 力を保存（伝播処理用）
        Forces[i] = Force;

        // 力の上限を適用（安全リミッター）
        float MaxForce = V.bIsCore ? MaxCoreForce : MaxForcePerVertex;
        if (Forces[i].SizeSquared() > MaxForce * MaxForce)
        {
            Forces[i] = Forces[i].GetSafeNormal() * MaxForce;
        }
    }

    // 力の伝播処理
    if (bEnableForcePropagate && Contacts.Num() > 0)  // 接触がある時だけ伝播
    {
        PropagateForces(Forces, DeltaTime);
    }

    // 力を適用して位置を更新
    for (int32 i = 0; i < Vertices.Num(); i++)
    {
        FSlimeVertex& V = Vertices[i];

        /* 積分 */
        V.Velocity += Forces[i] * DeltaTime;
        V.Velocity *= Damping;
        V.Position += V.Velocity * DeltaTime;

        NewPositions.Add(V.Position);
    }

    /* 接触の減衰 */
    for (FSlimeContact& C : Contacts)
    {
        C.Strength *= ContactDecayRate;
    }

    Contacts.RemoveAll(
        [](const FSlimeContact& C)
        {
            return C.Strength < 1.f;
        }
    );

    Mesh->UpdateMeshSection(0, NewPositions, {}, {}, {}, {});
    //Mesh->UpdateCollision();
}

/* ===============================
   Force Propagation
================================ */

void USlimeFluidComponent::PropagateForces(TArray<FVector>& Forces, float DeltaTime)
{
    // 力の伝播を複数回反復
    for (int32 Iteration = 0; Iteration < PropagationIterations; Iteration++)
    {
        TArray<FVector> PropagatedForces = Forces; // コピーを作成

        for (int32 i = 0; i < Vertices.Num(); i++)
        {
            const FSlimeVertex& V = Vertices[i];

            // コア頂点は伝播しない
            if (V.bIsCore)
            {
                continue;
            }

            // 隣接頂点が存在しない場合はスキップ
            if (i >= VertexNeighbors.Num() || VertexNeighbors[i].Neighbors.Num() == 0)
            {
                continue;
            }

            const TArray<int32>& Neighbors = VertexNeighbors[i].Neighbors;
            FVector AccumulatedForce = FVector::ZeroVector;
            int32 ValidNeighborCount = 0;

            // 隣接頂点から力を集める
            for (int32 NeighborIdx : Neighbors)
            {
                if (NeighborIdx >= 0 && NeighborIdx < Vertices.Num())
                {
                    const FSlimeVertex& NeighborV = Vertices[NeighborIdx];

                    // 隣接頂点の力を取得
                    FVector NeighborForce = Forces[NeighborIdx];

                    // 距離に応じた重み付け
                    float Distance = (V.Position - NeighborV.Position).Size();
                    float Weight = 1.0f / FMath::Max(Distance, 1.0f);

                    AccumulatedForce += NeighborForce * Weight;
                    ValidNeighborCount++;
                }
            }

            // 平均化して伝播
            if (ValidNeighborCount > 0)
            {
                FVector PropagatedForce = AccumulatedForce / ValidNeighborCount;
                PropagatedForce *= PropagationDamping; // 減衰

                // 元の力に伝播した力を追加
                PropagatedForces[i] += PropagatedForce * PropagationStrength * V.SurfaceWeight;
            }
        }

        // 更新された力を反映
        Forces = PropagatedForces;
    }
}

/* ===============================
   Core Center Update
================================ */

void USlimeFluidComponent::UpdateCoreCenter(float DeltaTime)
{
    // 表面頂点の平均位置を計算
    FVector CurrentCenter = FVector::ZeroVector;
    int32 SurfaceVertexCount = 0;

    for (const FSlimeVertex& V : Vertices)
    {
        if (!V.bIsCore) // 表面頂点のみで重心計算
        {
            CurrentCenter += V.Position;
            SurfaceVertexCount++;
        }
    }

    if (SurfaceVertexCount > 0)
    {
        CurrentCenter /= SurfaceVertexCount;
    }

    // ターゲットとなるセンター位置を決定
    FVector TargetCenter;

    if (bUseWorldOrigin)
    {
        // ワールド座標でオーナーの位置を基準にする
        FTransform T = GetComponentTransform();
        FVector OwnerLocation = GetOwner() ? GetOwner()->GetActorLocation() : FVector::ZeroVector;
        TargetCenter = T.InverseTransformPosition(OwnerLocation);
    }
    else
    {
        // ローカル座標の原点(0,0,0)を基準にする
        TargetCenter = FVector::ZeroVector;
    }

    // センターの移動ロジック
    bool bIsDeforming = Contacts.Num() > 0;

    if (bAllowCenterMovement && bIsDeforming)
    {
        // 変形中：表面の重心に追従（ただし制限付き）
        // センターの移動量を制限して、過度な移動を防ぐ
        FVector DesiredCenter = CurrentCenter;
        float MaxCenterShift = Radius * 0.3f; // 最大でも半径の30%まで
        FVector CenterOffset = DesiredCenter - TargetCenter;

        if (CenterOffset.Size() > MaxCenterShift)
        {
            CenterOffset = CenterOffset.GetSafeNormal() * MaxCenterShift;
            DesiredCenter = TargetCenter + CenterOffset;
        }

        LocalCoreCenter = FMath::VInterpTo(
            LocalCoreCenter,
            DesiredCenter,
            DeltaTime,
            CoreFollowStrength
        );
    }
    else if (bResetCenterToOrigin)
    {
        // 変形していない、または移動を許可しない場合：原点に戻る
        LocalCoreCenter = FMath::VInterpTo(
            LocalCoreCenter,
            TargetCenter,
            DeltaTime,
            CenterResetSpeed
        );
    }
    else if (!bAllowCenterMovement)
    {
        // センター移動を許可しない場合：常に原点に固定
        LocalCoreCenter = TargetCenter;
    }
}

/* ===============================
   Debug Visualization
================================ */

void USlimeFluidComponent::DrawDebugVisualization()
{
    if (!bShowVertexDebug || !GetWorld())
    {
        return;
    }

    FTransform T = GetComponentTransform();

    // 初期形状の表示（緑色のワイヤーフレーム球）
    if (bShowInitialShape && InitialVertexPositions.Num() > 0)
    {
        for (int32 i = 0; i < Triangles.Num(); i += 3)
        {
            FVector P0 = T.TransformPosition(InitialVertexPositions[Triangles[i]]);
            FVector P1 = T.TransformPosition(InitialVertexPositions[Triangles[i + 1]]);
            FVector P2 = T.TransformPosition(InitialVertexPositions[Triangles[i + 2]]);

            DrawDebugLine(GetWorld(), P0, P1, FColor::Green, false, -1.0f, 0, 0.5f);
            DrawDebugLine(GetWorld(), P1, P2, FColor::Green, false, -1.0f, 0, 0.5f);
            DrawDebugLine(GetWorld(), P2, P0, FColor::Green, false, -1.0f, 0, 0.5f);
        }
    }

    // 実際のメッシュ形状の表示（白色のワイヤーフレーム）
    if (bShowActualMeshShape)
    {
        for (int32 i = 0; i < Triangles.Num(); i += 3)
        {
            FVector P0 = T.TransformPosition(Vertices[Triangles[i]].Position);
            FVector P1 = T.TransformPosition(Vertices[Triangles[i + 1]].Position);
            FVector P2 = T.TransformPosition(Vertices[Triangles[i + 2]].Position);

            DrawDebugLine(GetWorld(), P0, P1, FColor::White, false, -1.0f, 0, 1.5f);
            DrawDebugLine(GetWorld(), P1, P2, FColor::White, false, -1.0f, 0, 1.5f);
            DrawDebugLine(GetWorld(), P2, P0, FColor::White, false, -1.0f, 0, 1.5f);
        }
    }

    // コア中心の表示
    if (bShowCoreCenter)
    {
        FVector WorldCoreCenter = T.TransformPosition(LocalCoreCenter);
        DrawDebugSphere(
            GetWorld(),
            WorldCoreCenter,
            10.0f,
            12,
            FColor::Yellow,
            false,
            -1.0f,
            0,
            2.0f
        );

        // 座標表示（ワールド/ローカル切り替え）
        FString CoordText;
        if (bShowWorldCoordinates)
        {
            CoordText = FString::Printf(TEXT("Core Center (World): %s"), *WorldCoreCenter.ToString());
        }
        else
        {
            CoordText = FString::Printf(TEXT("Core Center (Local): %s"), *LocalCoreCenter.ToString());
        }

        DrawDebugString(
            GetWorld(),
            WorldCoreCenter + FVector(0, 0, 20),
            CoordText,
            nullptr,
            FColor::Yellow,
            0.0f,
            true
        );
    }

    // 頂点の表示
    for (int32 i = 0; i < Vertices.Num(); i++)
    {
        const FSlimeVertex& V = Vertices[i];
        FVector WorldPos = T.TransformPosition(V.Position);

        // 表示する座標を選択
        FVector DisplayPos = bShowWorldCoordinates ? WorldPos : V.Position;

        // コア頂点
        if (V.bIsCore && bShowCoreVertices)
        {
            DrawDebugPoint(
                GetWorld(),
                WorldPos,
                DebugPointSize,
                FColor::Red,
                false,
                -1.0f,
                0
            );

            // 一部の頂点に詳細情報を表示（全部だと重い）
            if (i % 20 == 0)
            {
                FString CoordType = bShowWorldCoordinates ? TEXT("World") : TEXT("Local");
                DrawDebugString(
                    GetWorld(),
                    WorldPos,
                    FString::Printf(TEXT("C[%d] (%s)\nPos:%s\nVel:%.1f"),
                        i,
                        *CoordType,
                        *DisplayPos.ToCompactString(),
                        V.Velocity.Size()),
                    nullptr,
                    FColor::Red,
                    0.0f,
                    true,
                    0.8f
                );
            }
        }
        // 表面頂点
        else if (!V.bIsCore && bShowSurfaceVertices)
        {
            // SurfaceWeightに応じて色を変化
            FColor VertexColor = FColor::MakeRedToGreenColorFromScalar(1.0f - V.SurfaceWeight);

            DrawDebugPoint(
                GetWorld(),
                WorldPos,
                DebugPointSize,
                VertexColor,
                false,
                -1.0f,
                0
            );

            // 一部の頂点に詳細情報を表示
            if (i % 25 == 0)
            {
                FString CoordType = bShowWorldCoordinates ? TEXT("World") : TEXT("Local");
                DrawDebugString(
                    GetWorld(),
                    WorldPos,
                    FString::Printf(TEXT("S[%d] (%s)\nPos:%s\nWeight:%.2f\nVel:%.1f"),
                        i,
                        *CoordType,
                        *DisplayPos.ToCompactString(),
                        V.SurfaceWeight,
                        V.Velocity.Size()),
                    nullptr,
                    VertexColor,
                    0.0f,
                    true,
                    0.8f
                );
            }
        }
    }

    // 接触点の表示
    if (bShowContactPoints)
    {
        for (int32 i = 0; i < Contacts.Num(); i++)
        {
            const FSlimeContact& C = Contacts[i];
            FVector WorldContactPos = T.TransformPosition(C.LocalPosition);
            FVector WorldNormal = T.TransformVector(C.Normal);

            // 表示する座標を選択
            FVector DisplayContactPos = bShowWorldCoordinates ? WorldContactPos : C.LocalPosition;

            // 接触点
            DrawDebugSphere(
                GetWorld(),
                WorldContactPos,
                8.0f,
                8,
                FColor::Magenta,
                false,
                -1.0f,
                0,
                2.0f
            );

            // 接触法線
            DrawDebugDirectionalArrow(
                GetWorld(),
                WorldContactPos,
                WorldContactPos + WorldNormal * 30.0f,
                20.0f,
                FColor::Cyan,
                false,
                -1.0f,
                0,
                3.0f
            );

            // 影響範囲
            DrawDebugSphere(
                GetWorld(),
                WorldContactPos,
                Radius * ContactInfluenceRadius,
                16,
                FColor(255, 0, 255, 30),
                false,
                -1.0f,
                0,
                1.0f
            );

            // 接触情報
            FString CoordType = bShowWorldCoordinates ? TEXT("World") : TEXT("Local");
            DrawDebugString(
                GetWorld(),
                WorldContactPos + FVector(0, 0, 15),
                FString::Printf(TEXT("Contact[%d] (%s)\nPos:%s\nStrength:%.1f\nInfluence Radius: %.1f"),
                    i,
                    *CoordType,
                    *DisplayContactPos.ToCompactString(),
                    C.Strength,
                    Radius * ContactInfluenceRadius),
                nullptr,
                FColor::Magenta,
                0.0f,
                true
            );
        }
    }

    // 統計情報
    FVector InfoPos = T.GetLocation() + FVector(0, 0, Radius + 50);
    FString CoordSystemText = bShowWorldCoordinates ? TEXT("World") : TEXT("Local");

    // センター制御の状態表示
    FString CenterControlText;
    if (bAllowCenterMovement)
    {
        CenterControlText = TEXT("Center Movement: ENABLED");
    }
    else
    {
        CenterControlText = TEXT("Center Movement: LOCKED");
    }

    FString ResetText;
    if (bResetCenterToOrigin)
    {
        if (bUseWorldOrigin)
        {
            ResetText = FString::Printf(TEXT("Reset To: Owner Position (Speed: %.1f)"), CenterResetSpeed);
        }
        else
        {
            ResetText = FString::Printf(TEXT("Reset To: Local Origin (Speed: %.1f)"), CenterResetSpeed);
        }
    }
    else
    {
        ResetText = TEXT("Reset: DISABLED");
    }

    // 物理設定の状態
    FString CollisionMode = bUseMeshBasedCollision ? TEXT("Mesh-Based") : TEXT("Fixed-Direction");
    FString VolumeMode = bPreserveVolume ? TEXT("ON") : TEXT("OFF");
    FString PenetrationMode = bPreventPenetration ? TEXT("ON") : TEXT("OFF");
    FString GroundHandlingMode = bEnableGroundSpecialHandling ? TEXT("ON") : TEXT("OFF");

    // 地面接触数をカウント
    int32 GroundContactCount = 0;
    for (const FSlimeContact& C : Contacts)
    {
        if (C.bIsGround) GroundContactCount++;
    }

    DrawDebugString(
        GetWorld(),
        InfoPos,
        FString::Printf(TEXT("=== Slime Debug Info ===\nCoordinate System: %s\n%s\n%s\nCollision: %s\nVolume Preservation: %s\nPenetration Prevention: %s\nGround Special Handling: %s\nTotal Vertices: %d\nCore Vertices: %d\nSurface Vertices: %d\nActive Contacts: %d (Ground: %d)\nAvg Radius: %.1f\nTarget Radius: %.1f"),
            *CoordSystemText,
            *CenterControlText,
            *ResetText,
            *CollisionMode,
            *VolumeMode,
            *PenetrationMode,
            *GroundHandlingMode,
            Vertices.Num(),
            Vertices.FilterByPredicate([](const FSlimeVertex& V) { return V.bIsCore; }).Num(),
            Vertices.FilterByPredicate([](const FSlimeVertex& V) { return !V.bIsCore; }).Num(),
            Contacts.Num(),
            GroundContactCount,
            [this]() {
                float Sum = 0.f;
                for (const FSlimeVertex& V : Vertices) Sum += V.Position.Size();
                return Sum / Vertices.Num();
            }(),
                TargetAverageRadius),
        nullptr,
        FColor::White,
        0.0f,
        true,
        1.2f
    );
}