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

    // 慣性システムの初期化
    PreviousWorldLocation = GetComponentLocation();
    PreviousWorldVelocity = FVector::ZeroVector;
    CurrentAcceleration = FVector::ZeroVector;
    InertialCoreOffset = FVector::ZeroVector;
    InertialOffsetVelocity = FVector::ZeroVector;
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

    // 慣性システムの更新（最優先）
    UpdateInertialSystem(DeltaTime);

    DetectAllContacts();
    UpdateFluid(DeltaTime);
    DrawDebugVisualization();
}

void USlimeFluidComponent::UpdateInertialSystem(float DeltaTime)
{
    if (DeltaTime <= 0.0f || DeltaTime > 0.1f) return;

    // 現在のワールド位置
    FVector CurrentWorldLocation = GetComponentLocation();

    // 速度を計算
    FVector CurrentWorldVelocity = (CurrentWorldLocation - PreviousWorldLocation) / DeltaTime;

    // 加速度を計算
    FVector InstantAcceleration = (CurrentWorldVelocity - PreviousWorldVelocity) / DeltaTime;
    CurrentAcceleration = InstantAcceleration;

    // ローカル空間に変換
    FTransform T = GetComponentTransform();
    FVector LocalVelocity = T.InverseTransformVector(CurrentWorldVelocity);
    FVector LocalAcceleration = T.InverseTransformVector(CurrentAcceleration);

    // 慣性力を計算（加速度と逆方向）
    FVector InertialForce = -LocalAcceleration * InertiaDeformationStrength;

    // 慣性オフセットに力を適用（バネ-ダンパー系）
    InertialOffsetVelocity += InertialForce * DeltaTime;
    InertialOffsetVelocity *= InertialOffsetDamping;
    InertialCoreOffset += InertialOffsetVelocity * DeltaTime;

    // オフセットの上限
    if (InertialCoreOffset.SizeSquared() > MaxInertialOffset * MaxInertialOffset)
    {
        InertialCoreOffset = InertialCoreOffset.GetSafeNormal() * MaxInertialOffset;
    }

    // 次フレーム用に保存
    PreviousWorldLocation = CurrentWorldLocation;
    PreviousWorldVelocity = CurrentWorldVelocity;
}


void USlimeFluidComponent::UpdateAcceleration(float DeltaTime)
{
    if (DeltaTime <= 0.0f || DeltaTime > 0.1f) return; // 異常なDeltaTimeを除外

    // 現在のワールド位置
    FVector CurrentWorldLocation = GetComponentLocation();

    // 速度を計算（位置の変化率）
    FVector CurrentWorldVelocity = (CurrentWorldLocation - PreviousWorldLocation) / DeltaTime;

    // 加速度を計算（速度の変化率）
    // より正確にするため、前フレームと前々フレームの速度も考慮
    FVector InstantAcceleration = (CurrentWorldVelocity - PreviousWorldVelocity) / DeltaTime;

    // 加速度履歴に追加
    AccelerationHistory.RemoveAt(0);
    AccelerationHistory.Add(InstantAcceleration);

    // 平均加速度を計算（ノイズ除去）
    FVector AverageAcceleration = FVector::ZeroVector;
    for (const FVector& Accel : AccelerationHistory)
    {
        AverageAcceleration += Accel;
    }
    AverageAcceleration /= AccelerationHistory.Num();

    // 現在の加速度として保存
    CurrentAcceleration = AverageAcceleration;

    // 次フレーム用に保存
    PreviousWorldLocation = CurrentWorldLocation;
    PreviousPreviousWorldVelocity = PreviousWorldVelocity;
    PreviousWorldVelocity = CurrentWorldVelocity;
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
    UpdateCoreCenter(DeltaTime);

    // 平均半径の計算
    float CurrentAverageRadius = 0.f;
    for (const FSlimeVertex& V : Vertices)
    {
        CurrentAverageRadius += V.Position.Size();
    }
    CurrentAverageRadius /= Vertices.Num();
    float RadiusError = TargetAverageRadius - CurrentAverageRadius;

    // === 動的な状態判定 ===
    FTransform T = GetComponentTransform();
    FVector LocalVelocity = T.InverseTransformVector(PreviousWorldVelocity);
    FVector LocalAcceleration = T.InverseTransformVector(CurrentAcceleration);

    float VerticalVelocity = LocalVelocity.Z;
    float VerticalAccel = LocalAcceleration.Z;
    FVector HorizontalVelocity = FVector(LocalVelocity.X, LocalVelocity.Y, 0.0f);
    float HorizontalSpeed = HorizontalVelocity.Size();

    // ジャンプ判定を改善
    bool bWasJumping = bIsJumping;
    bIsJumping = (VerticalVelocity > FallingVelocityThreshold * 0.5f) && (VerticalAccel > JumpAccelerationThreshold * 0.3f);
    bIsFalling = (VerticalVelocity < -FallingVelocityThreshold);
    bIsLanding = bIsFalling && (VerticalAccel > LandingAccelerationThreshold);
    bool bIsMovingFast = (HorizontalSpeed > 100.0f);

    // デバッグ表示
    if (bShowAccelerationDebug && GEngine)
    {
        GEngine->AddOnScreenDebugMessage(
            1, 0.0f, FColor::Yellow,
            FString::Printf(TEXT("Vel: %.1f, %.1f, %.1f | Accel: %.1f, %.1f, %.1f"),
                LocalVelocity.X, LocalVelocity.Y, LocalVelocity.Z,
                LocalAcceleration.X, LocalAcceleration.Y, LocalAcceleration.Z)
        );

        GEngine->AddOnScreenDebugMessage(
            2, 0.0f, FColor::Cyan,
            FString::Printf(TEXT("HSpeed: %.1f | Moving: %s"),
                HorizontalSpeed,
                bIsMovingFast ? TEXT("YES") : TEXT("NO"))
        );

        GEngine->AddOnScreenDebugMessage(
            3, 0.0f, FColor::Green,
            FString::Printf(TEXT("State: %s%s%s%s"),
                bIsFalling ? TEXT("[FALL] ") : TEXT(""),
                bIsLanding ? TEXT("[LAND] ") : TEXT(""),
                bIsJumping ? TEXT("[JUMP] ") : TEXT(""),
                bIsMovingFast ? TEXT("[FAST]") : TEXT(""))
        );
    }

    // 地面接触の検出
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
                break;
            }
        }
    }

    // === 変形の「目標コア位置」を計算 ===
    FVector TargetCorePosition = LocalCoreCenter + InertialCoreOffset;

    // 移動方向の計算(ローカル空間)
    FVector MovementDirection = FVector::ZeroVector;
    bool bIsMoving = (HorizontalSpeed > VelocityThreshold);
    if (bIsMoving)
    {
        MovementDirection = HorizontalVelocity.GetSafeNormal();
    }

    // === 頂点ごとのシミュレーション ===
    TArray<FVector> Forces;
    Forces.SetNum(Vertices.Num());
    TArray<FVector> NewPositions;
    NewPositions.Reserve(Vertices.Num());

    for (int32 i = 0; i < Vertices.Num(); i++)
    {
        FSlimeVertex& V = Vertices[i];
        FVector Force = FVector::ZeroVector;

        if (V.bIsCore)
        {
            // === コア頂点：目標コア位置に強く拘束 ===
            Force += (TargetCorePosition - V.Position) * CoreStiffness;
            V.Velocity *= 0.6f;
        }
        else // 表面頂点
        {
            float Stiffness = FMath::Lerp(SurfaceStiffness, SurfaceSoftness, V.SurfaceWeight);

            // === 1. 目標コア位置への基本拘束 ===
            FVector ToTargetCore = TargetCorePosition - V.Position;

            float BaseRecoverySpeed = RecoverySpeed;
            if (bIsMoving && !bIsJumping)
            {
                BaseRecoverySpeed *= MovementRecoverySpeedMultiplier;
            }
            if (bIsJumping)
            {
                BaseRecoverySpeed *= JumpRecoverySpeedMultiplier;
            }

            Force += ToTargetCore * Stiffness * BaseRecoverySpeed;

            // === 2. 🔧 ジャンプ時の楕円化（修正版） ===
            if (bIsJumping)
            {
                // 頂点の法線方向（球の中心からの方向）
                FVector RadialDir = V.Position.GetSafeNormal();

                // 法線のZ成分で上下を判定（-1=真下、+1=真上）
                float VerticalComponent = RadialDir.Z;

                float JumpStretchAmount = FMath::Abs(VerticalVelocity) * 0.12f * JumpStretchMultiplier;

                // Z方向の変形（放射方向に適用）
                // 下側（VerticalComponent < 0）：伸ばす
                // 上側（VerticalComponent > 0）：縮める
                float ZDeform = -VerticalComponent * JumpStretchAmount; // 符号を反転
                Force += RadialDir * ZDeform * V.SurfaceWeight;

                // XY方向：縮める（楕円化）
                // 赤道付近を強く縮める
                float EquatorInfluence = 1.0f - FMath::Abs(VerticalComponent);
                FVector XYDir = FVector(V.Position.X, V.Position.Y, 0.0f);
                if (!XYDir.IsNearlyZero())
                {
                    XYDir.Normalize();
                    float XYShrinkAmount = -JumpStretchAmount * 0.6f * JumpXYShrinkMultiplier;
                    Force += XYDir * XYShrinkAmount * EquatorInfluence * V.SurfaceWeight;
                }
            }
            // 着地時の潰し
            else if (bIsLanding)
            {
                FVector RadialDir = V.Position.GetSafeNormal();
                float VerticalComponent = RadialDir.Z;

                float SquashAmount = Radius * 0.3f * LandingSquashMultiplier;

                // 下側を強く潰す（放射方向に圧縮）
                if (VerticalComponent < 0.0f)
                {
                    Force += RadialDir * (-SquashAmount * FMath::Abs(VerticalComponent)) * V.SurfaceWeight;
                }

                // 横に広がる（赤道付近）
                float EquatorInfluence = 1.0f - FMath::Abs(VerticalComponent);
                FVector XYDir = FVector(V.Position.X, V.Position.Y, 0.0f);
                if (!XYDir.IsNearlyZero())
                {
                    XYDir.Normalize();
                    Force += XYDir * (SquashAmount * 0.5f) * EquatorInfluence * V.SurfaceWeight;
                }
            }
            // 落下時の伸び
            else if (bIsFalling)
            {
                FVector RadialDir = V.Position.GetSafeNormal();
                float VerticalComponent = RadialDir.Z;

                float StretchAmount = Radius * 0.2f * FallingStretchMultiplier;

                // 下側を伸ばす
                if (VerticalComponent < 0.0f)
                {
                    Force += RadialDir * (StretchAmount * FMath::Abs(VerticalComponent)) * V.SurfaceWeight;
                }

                // 横を縮める
                float EquatorInfluence = 1.0f - FMath::Abs(VerticalComponent);
                FVector XYDir = FVector(V.Position.X, V.Position.Y, 0.0f);
                if (!XYDir.IsNearlyZero())
                {
                    XYDir.Normalize();
                    Force += XYDir * (-StretchAmount * 0.3f) * EquatorInfluence * V.SurfaceWeight;
                }
            }

            // === 3. 🔧 移動による前後の変形（停止時の角問題を修正） ===
            // bIsMoving の代わりに HorizontalSpeed を直接使用
            if (HorizontalSpeed > VelocityThreshold && !bIsJumping)
            {
                // 頂点の水平方向成分
                FVector VertexHorizontal = FVector(V.Position.X, V.Position.Y, 0.0f);
                float VertexHorizontalDist = VertexHorizontal.Size();

                if (VertexHorizontalDist > KINDA_SMALL_NUMBER && !MovementDirection.IsNearlyZero())
                {
                    VertexHorizontal /= VertexHorizontalDist;

                    // 進行方向との内積 (+1=前方, -1=後方)
                    float DirectionAlignment = FVector::DotProduct(VertexHorizontal, MovementDirection);

                    // 🔧 速度に比例した変形強度（停止時は自動的にゼロに）
                    float SpeedFactor = FMath::Clamp(HorizontalSpeed / 500.0f, 0.0f, 1.0f);
                    float MovementDeformStrength = HorizontalSpeed * 0.1f * MovementDeformMultiplier * SpeedFactor;

                    // 前後方向の変形（速度が低いと自動的に弱まる）
                    if (MovementDeformStrength > 0.1f)
                    {
                        FVector MovementDeform = MovementDirection * DirectionAlignment * MovementDeformStrength;

                        if (DirectionAlignment < -0.2f) // 後方
                        {
                            MovementDeform *= BackSquashMultiplier;
                        }
                        else if (DirectionAlignment > 0.2f) // 前方
                        {
                            MovementDeform *= FrontStretchMultiplier;
                        }

                        Force += MovementDeform * V.SurfaceWeight;
                    }

                    // === 後ろ側を小さくする ===
                    if (DirectionAlignment < 0.0f && MovementDeformStrength > 0.1f)
                    {
                        float ShrinkFactor = FMath::Abs(DirectionAlignment);
                        ShrinkFactor = FMath::Pow(ShrinkFactor, 1.2f);

                        // 中心に向かう力
                        FVector ToCenter = -V.Position.GetSafeNormal();
                        float ShrinkStrength = MovementDeformStrength * 1.5f * BackShrinkMultiplier * SpeedFactor;
                        Force += ToCenter * ShrinkStrength * ShrinkFactor * V.SurfaceWeight;

                        // 半径方向にも縮める
                        FVector RadialDir = V.Position.GetSafeNormal();
                        Force += RadialDir * (-ShrinkStrength * 0.5f) * ShrinkFactor * V.SurfaceWeight;
                    }
                    else if (DirectionAlignment >= 0.0f && MovementDeformStrength > 0.1f)
                    {
                        // 体積補償
                        float ExpansionFactor = 1.0f - DirectionAlignment;
                        ExpansionFactor = FMath::Pow(ExpansionFactor, 0.7f);

                        FVector Outward = V.Position.GetSafeNormal();
                        float ExpansionStrength = MovementDeformStrength * 0.8f * FrontExpansionMultiplier * SpeedFactor;
                        Force += Outward * ExpansionStrength * ExpansionFactor * V.SurfaceWeight;
                    }

                    // 垂直方向への影響
                    if (MovementDeformStrength > 0.1f)
                    {
                        float VerticalInfluence = DirectionAlignment * MovementDeformStrength * MovementVerticalInfluence;
                        Force.Z += VerticalInfluence * V.SurfaceWeight;
                    }
                }
            }

            // === 4. 半径保持 ===
            if (bPreserveVolume)
            {
                Force += V.Normal * RadiusError * 0.6f * BaseRecoverySpeed;
            }

            // === 5. 追従ラグ ===
            float LagStrength = FollowLagStrength * V.SurfaceWeight * 0.15f;
            Force += -V.Velocity * LagStrength;

            // === 6. ノイズとジグル ===
            FVector Noise(
                FMath::PerlinNoise1D(GetWorld()->TimeSeconds * 3.0f + V.Position.X * 0.1f),
                FMath::PerlinNoise1D(GetWorld()->TimeSeconds * 3.0f + V.Position.Y * 0.1f),
                FMath::PerlinNoise1D(GetWorld()->TimeSeconds * 3.0f + V.Position.Z * 0.1f)
            );
            Force += Noise * NoiseStrength * V.SurfaceWeight * 0.1f;

            FVector JiggleForce = V.Velocity * JiggleAmount * V.SurfaceWeight * 0.15f;
            Force += JiggleForce;

            // === 7. バウンス ===
            if (V.Velocity.Size() > 1.0f)
            {
                FVector BounceForce = V.Velocity.GetSafeNormal() * V.Velocity.SizeSquared() * 0.01f * BounceFactor;
                Force += BounceForce * V.SurfaceWeight;
            }

            // === 8. めり込み防止 ===
            if (bPreventPenetration)
            {
                for (const FSlimeContact& C : Contacts)
                {
                    FVector ToV = V.Position - C.LocalPosition;
                    float DistToContact = FVector::DotProduct(ToV, C.Normal);

                    if (DistToContact < 0.0f)
                    {
                        Force += C.Normal * (-DistToContact) * PenetrationResistance;
                    }
                }
            }

            // === 9. 接触による変形 ===
            for (const FSlimeContact& C : Contacts)
            {
                FVector ToV = V.Position - C.LocalPosition;
                float Dist = ToV.Size();
                float InfluenceRadius = Radius * ContactInfluenceRadius;

                if (Dist < InfluenceRadius && Dist > KINDA_SMALL_NUMBER)
                {
                    FVector Dir = ToV / Dist;
                    float DirectionDot = FVector::DotProduct(Dir, -C.Normal);

                    if (bPreventOppositeSideDeformation && DirectionDot < -0.1f)
                    {
                        continue;
                    }

                    float DistToContactPlane = FVector::DotProduct(ToV, C.Normal);
                    if (bPreventPenetration && DistToContactPlane < 0.0f)
                    {
                        continue;
                    }

                    float DirectionalWeight = FMath::Lerp(
                        1.0f,
                        FMath::Max(0.0f, DirectionDot),
                        ContactDirectionality
                    );

                    float Falloff = 1.f - (Dist / InfluenceRadius);
                    Falloff = FMath::Pow(Falloff, 1.5f);
                    Falloff *= DirectionalWeight;

                    float FinalDeformationSpeed = DeformationSpeed * 0.5f;
                    if (C.bIsGround && bEnableGroundSpecialHandling)
                    {
                        FinalDeformationSpeed *= GroundSquashMultiplier;
                    }

                    float Compression = C.Strength * Falloff * V.SurfaceWeight * FinalDeformationSpeed;

                    Force += -C.Normal * Compression;

                    float VelocityDamping = 0.7f;
                    if (C.bIsGround && bEnableGroundSpecialHandling)
                    {
                        VelocityDamping = 0.9f;
                    }
                    V.Velocity -= FVector::DotProduct(V.Velocity, C.Normal) * C.Normal * VelocityDamping;

                    FVector Tangent = Dir - C.Normal * FVector::DotProduct(Dir, C.Normal);
                    if (!Tangent.IsNearlyZero())
                    {
                        float SpreadWeight = FMath::Max(0.0f, DirectionDot);
                        float FinalSpreadStrength = SpreadStrength;

                        if (C.bIsGround && bEnableGroundSpecialHandling)
                        {
                            FinalSpreadStrength *= 1.3f;
                        }

                        Force += Tangent.GetSafeNormal() * Compression * FinalSpreadStrength * SpreadWeight;
                    }

                    if (C.bIsGround && bEnableGroundSpecialHandling && GroundStickiness > 0.0f)
                    {
                        FVector ToContact = C.LocalPosition - V.Position;
                        Force += ToContact * GroundStickiness * Falloff;
                    }
                }
            }
        }

        // === 底面の平面化 ===
        if (bHasGroundContact && bFlattenBottomOnGround && !V.bIsCore)
        {
            float DistToGround = FVector::DotProduct(V.Position - GroundPlanePosition, GroundPlaneNormal);

            if (DistToGround < Radius * BottomFlattenRadius)
            {
                FVector ProjectedPos = V.Position - GroundPlaneNormal * DistToGround;
                FVector FlattenForce = (ProjectedPos - V.Position) * BottomFlattenStrength;

                float FlattenWeight = 1.0f - (DistToGround / (Radius * BottomFlattenRadius));
                FlattenWeight = FMath::Clamp(FlattenWeight, 0.0f, 1.0f);
                FlattenWeight = FMath::Pow(FlattenWeight, 2.0f);

                Force += FlattenForce * FlattenWeight * V.SurfaceWeight;
            }
        }

        Forces[i] = Force;

        // 力の上限
        float MaxForce = V.bIsCore ? MaxCoreForce : MaxForcePerVertex;
        if (Forces[i].SizeSquared() > MaxForce * MaxForce)
        {
            Forces[i] = Forces[i].GetSafeNormal() * MaxForce;
        }
    }

    // 力の伝播
    if (bEnableForcePropagate && Contacts.Num() > 0)
    {
        PropagateForces(Forces, DeltaTime);
    }

    // 位置更新
    for (int32 i = 0; i < Vertices.Num(); i++)
    {
        FSlimeVertex& V = Vertices[i];

        V.Velocity += Forces[i] * DeltaTime;
        V.Velocity *= Damping;
        V.Position += V.Velocity * DeltaTime;

        NewPositions.Add(V.Position);
    }

    // 接触の減衰
    for (FSlimeContact& C : Contacts)
    {
        C.Strength *= ContactDecayRate;
    }

    Contacts.RemoveAll([](const FSlimeContact& C) { return C.Strength < 1.f; });

    Mesh->UpdateMeshSection(0, NewPositions, {}, {}, {}, {});
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

    if (bShowVertexDebug)
    {
        // 加速度の表示
        FVector WorldAccel = T.TransformVector(CurrentAcceleration);
        DrawDebugDirectionalArrow(
            GetWorld(),
            InfoPos,
            InfoPos + WorldAccel * 0.1f,
            30.0f,
            FColor::Orange,
            false, -1.0f, 0, 5.0f
        );

        // 状態表示
        FString StateText = FString::Printf(
            TEXT("Acceleration: %.1f\nState: %s"),
            CurrentAcceleration.Size(),
            bIsLanding ? TEXT("LANDING!") :
            bIsJumping ? TEXT("JUMPING!") :
            bIsFalling ? TEXT("FALLING") : TEXT("Normal")
        );
        //UE_LOG(LogTemp, Log, TEXT("CurrentAcceleration.x=%f,CurrentAcceleration.y=%f ,CurrentAcceleration.z=%f"), CurrentAcceleration.X, CurrentAcceleration.Y, CurrentAcceleration.Z);
    
        //UE_LOG(LogTemp, Warning, TEXT("The boolean value is %s"), (bIsLanding ? TEXT("bIsLanding::true") : TEXT("bIsLanding::false")));
        //UE_LOG(LogTemp, Warning, TEXT("The boolean value is %s"), (bIsJumping ? TEXT("bIsJumping::true") : TEXT("bIsJumping::false")));
        //UE_LOG(LogTemp, Warning, TEXT("The boolean value is %s"), (bIsFalling ? TEXT("bIsFalling::true") : TEXT("bIsFalling::false")));

    }
}