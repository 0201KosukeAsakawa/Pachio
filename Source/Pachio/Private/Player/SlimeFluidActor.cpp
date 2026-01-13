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

    // 流体移動システムの初期化
    VertexLagOffsets.SetNum(Vertices.Num());
    for (int32 i = 0; i < VertexLagOffsets.Num(); i++)
    {
        VertexLagOffsets[i] = FVector::ZeroVector;
    }

    InertialTilt = FRotator::ZeroRotator;
    InertialCenterOffset = FVector::ZeroVector;
    InertialVelocity = FVector::ZeroVector;

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

void USlimeFluidComponent::UpdateFluidMovement(float DeltaTime, const FVector& LocalVelocity, float HorizontalSpeed)
{
    if (HorizontalSpeed < VelocityThreshold)
    {
        // 静止時：傾きと遅延をリセット
        InertialTilt = FMath::RInterpTo(InertialTilt, FRotator::ZeroRotator, DeltaTime, TiltResponseSpeed);
        InertialCenterOffset = FMath::VInterpTo(InertialCenterOffset, FVector::ZeroVector, DeltaTime, LagPropagationSpeed);

        for (int32 i = 0; i < VertexLagOffsets.Num(); i++)
        {
            VertexLagOffsets[i] = FMath::VInterpTo(VertexLagOffsets[i], FVector::ZeroVector, DeltaTime, LagPropagationSpeed);
        }

        InertialVelocity *= CenterInertiaDamping;
        return;
    }

    // === 移動方向を取得 ===
    FVector MoveDir = FVector(LocalVelocity.X, LocalVelocity.Y, 0.0f).GetSafeNormal();

    // === 1. 体の傾き（進行方向に傾く） ===

    // 目標傾き：進行方向に傾ける
    float SpeedFactor = FMath::Clamp(HorizontalSpeed / 600.0f, 0.0f, 1.0f);
    float TiltAmount = MovementTiltAngle * SpeedFactor;

    // 進行方向の角度（Yaw）
    float MoveYaw = FMath::Atan2(MoveDir.Y, MoveDir.X) * 180.0f / PI;

    // 進行方向軸周りの傾き（ロール）
    FRotator TargetTilt(0.0f, 0.0f, 0.0f);
    TargetTilt.Pitch = -TiltAmount; // 前に傾く

    // 滑らかに補間
    InertialTilt = FMath::RInterpTo(InertialTilt, TargetTilt, DeltaTime, TiltResponseSpeed);

    // === 2. 重心の慣性移動（加速度と逆方向） ===

    FTransform T = GetComponentTransform();
    FVector LocalAcceleration = T.InverseTransformVector(CurrentAcceleration);

    // 慣性力（加速度と逆方向）
    FVector InertialForce = -LocalAcceleration * CenterInertiaStrength * 0.01f;

    // 速度を更新
    InertialVelocity += InertialForce * DeltaTime;
    InertialVelocity *= CenterInertiaDamping;

    // 位置を更新
    InertialCenterOffset += InertialVelocity * DeltaTime;

    // 上限
    float MaxOffset = Radius * 0.4f;
    if (InertialCenterOffset.SizeSquared() > MaxOffset * MaxOffset)
    {
        InertialCenterOffset = InertialCenterOffset.GetSafeNormal() * MaxOffset;
    }

    // === 3. 各頂点の遅延オフセット（後ろほど遅延） ===

    float CurrentTime = GetWorld()->TimeSeconds;

    for (int32 i = 0; i < Vertices.Num(); i++)
    {
        if (Vertices[i].bIsCore) continue; // コアは遅延なし

        FVector InitialPos = InitialVertexPositions.IsValidIndex(i)
            ? InitialVertexPositions[i]
            : Vertices[i].Position;

            // 進行方向との位置関係（-1=後方、0=側面、1=前方）
            FVector HorizontalPos = FVector(InitialPos.X, InitialPos.Y, 0.0f);
            float Alignment = FVector::DotProduct(HorizontalPos.GetSafeNormal(), MoveDir);

            // 後方ほど遅延が大きい
            if (Alignment < 0.0f) // 後方
            {
                float BackwardAmount = FMath::Abs(Alignment);

                // 遅延目標：進行方向と逆に引っ張る
                FVector LagTarget = -MoveDir * HorizontalSpeed * 0.1f * BackwardLagStrength * BackwardAmount;

                // 波打つ動き（縦方向の揺れ）
                float WavePhase = CurrentTime * WaveFrequency * PI * 2.0f;
                WavePhase += BackwardAmount * PI; // 後ろほど位相がずれる
                float WaveOffset = FMath::Sin(WavePhase) * WaveMotionStrength * BackwardAmount;
                LagTarget.Z += WaveOffset;

                // 滑らかに補間
                VertexLagOffsets[i] = FMath::VInterpTo(
                    VertexLagOffsets[i],
                    LagTarget,
                    DeltaTime,
                    LagPropagationSpeed
                );
            }
            else // 前方・側面
            {
                // 遅延を徐々に解消
                VertexLagOffsets[i] = FMath::VInterpTo(
                    VertexLagOffsets[i],
                    FVector::ZeroVector,
                    DeltaTime,
                    LagPropagationSpeed * 2.0f
                );
            }
    }
}

void USlimeFluidComponent::UpdateFluid(float DeltaTime)
{
    UpdateCoreCenter(DeltaTime);

    DeltaTime = FMath::Clamp(DeltaTime, 0.0f, 0.05f);

    FTransform T = GetComponentTransform();
    FVector LocalVelocity = T.InverseTransformVector(PreviousWorldVelocity);
    FVector LocalAcceleration = T.InverseTransformVector(CurrentAcceleration);

    float VerticalVelocity = LocalVelocity.Z;
    FVector HorizontalVelocity = FVector(LocalVelocity.X, LocalVelocity.Y, 0.0f);
    float HorizontalSpeed = HorizontalVelocity.Size();
    float VerticalAccel = LocalAcceleration.Z;

    bool bIsFalling = (VerticalVelocity < -FallingVelocityThreshold);
    bool bIsLanding = bIsFalling && (VerticalAccel > LandingAccelerationThreshold);
    bool bIsMoving = (HorizontalSpeed > VelocityThreshold);
    bool bIsJumping = (VerticalVelocity > 100.0f);

    // 移動方向
    FVector MoveDir = FVector::ZeroVector;
    if (bIsMoving)
    {
        MoveDir = HorizontalVelocity.GetSafeNormal();
    }

    // === 流体的な移動を更新 ===
    UpdateFluidMovement(DeltaTime, LocalVelocity, HorizontalSpeed);

    // デバッグ表示
    if (bShowAccelerationDebug && GEngine)
    {
        GEngine->AddOnScreenDebugMessage(1, 0.0f, FColor::Yellow,
            FString::Printf(TEXT("V: %.0f, %.0f, %.0f | HSpeed: %.0f"),
                LocalVelocity.X, LocalVelocity.Y, LocalVelocity.Z, HorizontalSpeed));

        GEngine->AddOnScreenDebugMessage(2, 0.0f, FColor::Cyan,
            FString::Printf(TEXT("Tilt: %.1f | Moving: %s"),
                InertialTilt.Pitch, bIsMoving ? TEXT("YES") : TEXT("NO")));
    }

    // 地面接触の検出
    bool bHasGroundContact = false;
    FVector GroundPlanePosition = FVector::ZeroVector;
    FVector GroundPlaneNormal = FVector::UpVector;

    if (bEnableGroundSpecialHandling)
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

    // === 目標コア位置（重心の慣性オフセットを適用） ===
    FVector TargetCorePosition = LocalCoreCenter + InertialCenterOffset;

    // === 頂点の更新 ===
    TArray<FVector> Forces;
    Forces.SetNum(Vertices.Num());
    TArray<FVector> NewPositions;
    NewPositions.Reserve(Vertices.Num());

    // 傾きの回転行列
    FQuat TiltQuat = InertialTilt.Quaternion();

    for (int32 i = 0; i < Vertices.Num(); i++)
    {
        FSlimeVertex& V = Vertices[i];
        FVector Force = FVector::ZeroVector;

        if (V.bIsCore)
        {
            FVector ToTarget = TargetCorePosition - V.Position;
            Force = ToTarget * CoreStiffness;
            V.Velocity *= 0.7f;
        }
        else // 表面頂点
        {
            // === 初期位置と方向を取得 ===
            FVector InitialPos = InitialVertexPositions.IsValidIndex(i)
                ? InitialVertexPositions[i]
                : V.Position;

                FVector InitialDir = InitialPos.GetSafeNormal();

                // === 1. ジャンプ/着地/落下の変形 ===
                FVector JumpStretch = FVector::ZeroVector;

                if (bIsJumping)
                {
                    float VerticalComponent = InitialDir.Z;
                    float JumpStretchAmount = FMath::Clamp(VerticalVelocity / 1000.0f, 0.0f, 0.5f) * JumpStretchMultiplier;

                    // 下側を伸ばす
                    JumpStretch = InitialDir * (-VerticalComponent * JumpStretchAmount * Radius);
                }
                else if (bIsLanding)
                {
                    float VerticalComponent = InitialDir.Z;
                    float SquashAmount = FMath::Clamp(FMath::Abs(VerticalAccel) / 2000.0f, 0.0f, 0.6f) * LandingSquashMultiplier;

                    // 下側を潰す
                    if (VerticalComponent < 0.0f)
                    {
                        JumpStretch = InitialDir * (VerticalComponent * SquashAmount * Radius);
                    }

                    // 横に広がる
                    float EquatorInfluence = 1.0f - FMath::Abs(VerticalComponent);
                    FVector XYDir = FVector(InitialPos.X, InitialPos.Y, 0.0f);
                    if (!XYDir.IsNearlyZero())
                    {
                        JumpStretch += XYDir.GetSafeNormal() * (SquashAmount * Radius * 0.5f * EquatorInfluence);
                    }
                }
                else if (bIsFalling)
                {
                    float VerticalComponent = InitialDir.Z;
                    float StretchAmount = FMath::Clamp(FMath::Abs(VerticalVelocity) / 1000.0f, 0.0f, 0.4f) * FallingStretchMultiplier;

                    if (VerticalComponent < 0.0f)
                    {
                        JumpStretch = InitialDir * (VerticalComponent * StretchAmount * Radius);
                    }
                }

                // === 2. 移動による変形（後ろを小さく、前・側面を大きく） ===
                FVector MovementDeform = FVector::ZeroVector;

                if (bIsMoving && !bIsJumping && !bIsLanding && !bIsFalling)
                {
                    // 頂点の水平方向成分
                    FVector VertexHorizontal = FVector(InitialPos.X, InitialPos.Y, 0.0f);

                    if (!VertexHorizontal.IsNearlyZero() && !MoveDir.IsNearlyZero())
                    {
                        VertexHorizontal.Normalize();

                        // 進行方向との内積 (+1=前方, -1=後方)
                        float DirectionAlignment = FVector::DotProduct(VertexHorizontal, MoveDir);

                        // 速度ファクター
                        float SpeedFactor = FMath::Clamp(HorizontalSpeed / 500.0f, 0.0f, 1.0f);

                        // === 後方：縮小 ===
                        if (DirectionAlignment < 0.0f)
                        {
                            float BackwardAmount = FMath::Abs(DirectionAlignment);
                            BackwardAmount = FMath::Pow(BackwardAmount, 1.2f);

                            // 半径方向に縮める（後ろを小さく）
                            float ShrinkStrength = HorizontalSpeed * 0.3f * BackShrinkMultiplier * SpeedFactor;
                            MovementDeform += InitialDir * (-ShrinkStrength * BackwardAmount);

                            // 進行方向と逆に引っ張る（引きずり感）
                            float DragStrength = HorizontalSpeed * 0.2f * BackwardLagStrength * SpeedFactor;
                            MovementDeform += -MoveDir * (DragStrength * BackwardAmount);
                        }
                        // === 前方：伸長 ===
                        else if (DirectionAlignment > 0.2f)
                        {
                            float ForwardAmount = DirectionAlignment;

                            // 進行方向に伸ばす
                            float StretchStrength = HorizontalSpeed * 0.15f * FrontStretchMultiplier * SpeedFactor;
                            MovementDeform += MoveDir * (StretchStrength * ForwardAmount);

                            // 半径方向にも少し伸ばす
                            MovementDeform += InitialDir * (StretchStrength * 0.3f * ForwardAmount);
                        }
                        // === 側面：膨張（体積補償） ===
                        else
                        {
                            // 側面の膨らみ（後方で失われた体積を補償）
                            float SideAmount = 1.0f - FMath::Abs(DirectionAlignment);
                            SideAmount = FMath::Pow(SideAmount, 0.8f);

                            float ExpansionStrength = HorizontalSpeed * 0.2f * FrontExpansionMultiplier * SpeedFactor;
                            MovementDeform += InitialDir * (ExpansionStrength * SideAmount);
                        }
                    }
                }

                // === 3. 傾きを適用 ===
                FVector TiltedDir = TiltQuat.RotateVector(InitialDir);

                // === 4. 目標位置を計算 ===
                // 基本位置 = 目標コア + 傾いた方向 × 半径
                FVector TargetPos = TargetCorePosition + TiltedDir * Radius;

                // 変形を追加
                TargetPos += JumpStretch;
                TargetPos += MovementDeform;

                // 遅延オフセットを追加（波打つ動き）
                TargetPos += VertexLagOffsets[i];

                // === 5. 復元力 ===
                FVector ToTarget = TargetPos - V.Position;
                float ShapeStiffness = FMath::Lerp(SurfaceStiffness, SurfaceSoftness, V.SurfaceWeight);

                // 移動中は復元を速く
                float EffectiveRecoverySpeed = RecoverySpeed;
                if (bIsMoving)
                {
                    EffectiveRecoverySpeed *= MovementRecoverySpeedMultiplier;
                }
                if (bIsJumping)
                {
                    EffectiveRecoverySpeed *= JumpRecoverySpeedMultiplier;
                }

                Force += ToTarget * ShapeStiffness * EffectiveRecoverySpeed;

                // === 6. 接触変形 ===
                for (const FSlimeContact& C : Contacts)
                {
                    FVector ToV = V.Position - C.LocalPosition;
                    float Dist = ToV.Size();
                    float InfluenceRadius = Radius * ContactInfluenceRadius;

                    if (Dist < InfluenceRadius && Dist > KINDA_SMALL_NUMBER)
                    {
                        FVector Dir = ToV / Dist;
                        float DirectionDot = FVector::DotProduct(Dir, -C.Normal);

                        if (DirectionDot > -0.1f)
                        {
                            float Falloff = 1.0f - (Dist / InfluenceRadius);
                            Falloff = FMath::Pow(Falloff, 2.0f);

                            float DirectionalWeight = FMath::Max(0.0f, DirectionDot);
                            Falloff *= FMath::Lerp(1.0f, DirectionalWeight, ContactDirectionality);

                            float ContactStrength = FMath::Min(C.Strength, 500.0f);
                            float PushStrength = ContactStrength * Falloff * V.SurfaceWeight * DeformationSpeed * 0.3f;
                            Force += -C.Normal * PushStrength;

                            FVector Tangent = Dir - C.Normal * FVector::DotProduct(Dir, C.Normal);
                            if (!Tangent.IsNearlyZero())
                            {
                                Force += Tangent.GetSafeNormal() * PushStrength * SpreadStrength * 0.5f;
                            }

                            float VelDamping = C.bIsGround ? 0.9f : 0.8f;
                            V.Velocity -= FVector::DotProduct(V.Velocity, C.Normal) * C.Normal * VelDamping * 0.5f;
                        }
                    }
                }

                // === 7. めり込み防止 ===
                if (bPreventPenetration)
                {
                    for (const FSlimeContact& C : Contacts)
                    {
                        FVector ToV = V.Position - C.LocalPosition;
                        float DistToPlane = FVector::DotProduct(ToV, C.Normal);
                        if (DistToPlane < 0.0f)
                        {
                            Force += C.Normal * (-DistToPlane) * PenetrationResistance;
                        }
                    }
                }

                // === 8. 地面平面化 ===
                if (bHasGroundContact && bFlattenBottomOnGround)
                {
                    float DistToGround = FVector::DotProduct(V.Position - GroundPlanePosition, GroundPlaneNormal);
                    if (DistToGround < Radius * BottomFlattenRadius)
                    {
                        FVector ProjectedPos = V.Position - GroundPlaneNormal * DistToGround;
                        float FlattenWeight = 1.0f - (DistToGround / (Radius * BottomFlattenRadius));
                        FlattenWeight = FMath::Pow(FlattenWeight, 2.0f);
                        FVector FlattenForce = (ProjectedPos - V.Position) * BottomFlattenStrength;
                        Force += FlattenForce * FlattenWeight * V.SurfaceWeight;
                    }
                }

                // === 9. 体積保存 ===
                if (bPreserveVolume)
                {
                    float CurrentRadius = V.Position.Size();
                    float RadiusError = Radius - CurrentRadius;
                    Force += V.Normal * RadiusError * 0.4f * EffectiveRecoverySpeed;
                }

                // === 10. ラグ・ノイズ・ジグル・バウンス ===
                Force += -V.Velocity * FollowLagStrength * V.SurfaceWeight * 0.2f;

                FVector Noise(
                    FMath::PerlinNoise1D(GetWorld()->TimeSeconds * 3.0f + V.Position.X * 0.1f),
                    FMath::PerlinNoise1D(GetWorld()->TimeSeconds * 3.0f + V.Position.Y * 0.1f),
                    FMath::PerlinNoise1D(GetWorld()->TimeSeconds * 3.0f + V.Position.Z * 0.1f)
                );
                Force += Noise * NoiseStrength * V.SurfaceWeight * 0.05f;

                if (V.Velocity.Size() > 5.0f)
                {
                    Force += V.Velocity * JiggleAmount * V.SurfaceWeight * 0.1f;
                }

                if (V.Velocity.Size() > 10.0f)
                {
                    FVector BounceForce = V.Velocity.GetSafeNormal() * V.Velocity.SizeSquared() * 0.005f * BounceFactor;
                    Force += BounceForce * V.SurfaceWeight;
                }
        }

        Forces[i] = Force;

        float MaxForce = V.bIsCore ? MaxCoreForce : MaxForcePerVertex;
        if (Forces[i].SizeSquared() > MaxForce * MaxForce)
        {
            Forces[i] = Forces[i].GetSafeNormal() * MaxForce;
        }
    }

    if (bEnableForcePropagate && Contacts.Num() > 0)
    {
        PropagateForces(Forces, DeltaTime);
    }

    // === 位置更新 ===
    for (int32 i = 0; i < Vertices.Num(); i++)
    {
        FSlimeVertex& V = Vertices[i];

        V.Velocity += Forces[i] * DeltaTime;
        V.Velocity *= Damping;

        float MaxVelocity = Radius * 20.0f;
        if (V.Velocity.SizeSquared() > MaxVelocity * MaxVelocity)
        {
            V.Velocity = V.Velocity.GetSafeNormal() * MaxVelocity;
        }

        V.Position += V.Velocity * DeltaTime;
        NewPositions.Add(V.Position);
    }

    // === 接触の減衰 ===
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
    }
}