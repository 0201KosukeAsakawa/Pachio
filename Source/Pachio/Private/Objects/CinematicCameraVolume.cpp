#include "Objects/CinematicCameraVolume.h"
#include "GameFramework/Character.h"
#include "DrawDebugHelpers.h"
#include "Kismet/KismetMathLibrary.h"

ACinematicCameraVolume::ACinematicCameraVolume()
{
    PrimaryActorTick.bCanEverTick = true;

    // ボリュームコンポーネント作成
    VolumeBox = CreateDefaultSubobject<UBoxComponent>(TEXT("VolumeBox"));
    RootComponent = VolumeBox;
    VolumeBox->SetBoxExtent(FVector(1000.f, 1000.f, 500.f));
    VolumeBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    VolumeBox->SetCollisionResponseToAllChannels(ECR_Ignore);
    VolumeBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

void ACinematicCameraVolume::BeginPlay()
{
    Super::BeginPlay();

    // オーバーラップイベント設定
    VolumeBox->OnComponentBeginOverlap.AddDynamic(this, &ACinematicCameraVolume::OnPlayerEnter);
    VolumeBox->OnComponentEndOverlap.AddDynamic(this, &ACinematicCameraVolume::OnPlayerExit);

    // 初期位置保存
    LastCameraLocation = GetActorLocation();
    LastCameraRotation = GetActorRotation();
}

void ACinematicCameraVolume::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // デバッグ表示
    if (bShowDebug)
    {
        FColor DebugColor = bIsActive ? FColor::Green : FColor::Yellow;
        DrawDebugBox(GetWorld(), GetActorLocation(), VolumeBox->GetScaledBoxExtent(),
            DebugColor, false, -1.f, 0, 5.f);

        // カメラタイプ表示
        FString TypeString;
        switch (CameraType)
        {
        case ECameraVolumeType::Fixed: TypeString = TEXT("Fixed"); break;
        case ECameraVolumeType::Follow: TypeString = TEXT("Follow"); break;
        case ECameraVolumeType::Rail: TypeString = TEXT("Rail"); break;
        case ECameraVolumeType::ConstrainedFollow: TypeString = TEXT("Constrained"); break;
        case ECameraVolumeType::DollyZoom: TypeString = TEXT("Dolly Zoom"); break;
        }

        DrawDebugString(GetWorld(), GetActorLocation() + FVector(0, 0, VolumeBox->GetScaledBoxExtent().Z + 100.f),
            FString::Printf(TEXT("%s (Priority: %d)"), *TypeString, Priority),
            nullptr, DebugColor, 0.f, true);

        // ウェイポイント表示（レールカメラ）
        if (CameraType == ECameraVolumeType::Rail)
        {
            for (int32 i = 0; i < Waypoints.Num(); ++i)
            {
                FVector WaypointWorld = GetActorTransform().TransformPosition(Waypoints[i].Location);
                DrawDebugSphere(GetWorld(), WaypointWorld, 50.f, 8, FColor::Cyan, false, -1.f, 0, 3.f);

                if (i < Waypoints.Num() - 1)
                {
                    FVector NextWaypointWorld = GetActorTransform().TransformPosition(Waypoints[i + 1].Location);
                    DrawDebugLine(GetWorld(), WaypointWorld, NextWaypointWorld, FColor::Cyan, false, -1.f, 0, 2.f);
                }
            }
        }
    }
}

void ACinematicCameraVolume::OnPlayerEnter(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (ACharacter* Character = Cast<ACharacter>(OtherActor))
    {
        bIsActive = true;
        UE_LOG(LogTemp, Log, TEXT("Player entered camera volume: %s"), *GetName());
    }
}

void ACinematicCameraVolume::OnPlayerExit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    if (ACharacter* Character = Cast<ACharacter>(OtherActor))
    {
        bIsActive = false;
        UE_LOG(LogTemp, Log, TEXT("Player exited camera volume: %s"), *GetName());
    }
}

void ACinematicCameraVolume::GetCameraTransform(const FVector& PlayerLocation, FVector& OutLocation, FRotator& OutRotation, float& OutFOV)
{
    OutFOV = CameraFOV;

    switch (CameraType)
    {
    case ECameraVolumeType::Fixed:
    {
        // 固定カメラ：ワールド空間での固定位置
        OutLocation = GetActorTransform().TransformPosition(FixedCameraLocation);
        OutRotation = (GetActorRotation() + FixedCameraRotation).Clamp();

        if (bLookAtPlayer)
        {
            FVector DirectionToPlayer = PlayerLocation - OutLocation;
            OutRotation = DirectionToPlayer.Rotation();
        }
        break;
    }

    case ECameraVolumeType::Follow:
    case ECameraVolumeType::ConstrainedFollow:
    {
        // 追従カメラ：プレイヤーに追従
        FVector TargetLocation = PlayerLocation + Constraints.FollowOffset;

        // 制限付き追従の場合、軸制限を適用
        if (CameraType == ECameraVolumeType::ConstrainedFollow)
        {
            FVector BaseLocation = GetActorLocation();

            if (!Constraints.bAllowMoveX)
                TargetLocation.X = BaseLocation.X;
            if (!Constraints.bAllowMoveY)
                TargetLocation.Y = BaseLocation.Y;
            if (!Constraints.bAllowMoveZ)
                TargetLocation.Z = BaseLocation.Z;

            // 範囲制限
            TargetLocation.X = FMath::Clamp(TargetLocation.X, Constraints.MinBounds.X, Constraints.MaxBounds.X);
            TargetLocation.Y = FMath::Clamp(TargetLocation.Y, Constraints.MinBounds.Y, Constraints.MaxBounds.Y);
            TargetLocation.Z = FMath::Clamp(TargetLocation.Z, Constraints.MinBounds.Z, Constraints.MaxBounds.Z);
        }

        // 遅延追従
        float LagSpeed = FMath::GetMappedRangeValueClamped(FVector2D(0.f, 1.f), FVector2D(10.f, 1.f), Constraints.FollowLag);
        OutLocation = FMath::VInterpTo(LastCameraLocation, TargetLocation, GetWorld()->GetDeltaSeconds(), LagSpeed);

        if (bLookAtPlayer)
        {
            FVector DirectionToPlayer = PlayerLocation - OutLocation;
            OutRotation = DirectionToPlayer.Rotation();
        }
        else
        {
            OutRotation = GetActorRotation();
        }
        break;
    }

    case ECameraVolumeType::Rail:
    {
        // レールカメラ：ウェイポイント間を補間
        if (Waypoints.Num() < 2)
        {
            OutLocation = GetActorLocation();
            OutRotation = GetActorRotation();
            break;
        }

        float Progress = GetPlayerProgress(PlayerLocation);

        // 現在の進行度に対応するウェイポイントを探す
        int32 CurrentWaypointIndex = 0;
        for (int32 i = 0; i < Waypoints.Num() - 1; ++i)
        {
            if (Progress >= Waypoints[i].Progress && Progress <= Waypoints[i + 1].Progress)
            {
                CurrentWaypointIndex = i;
                break;
            }
        }

        // 2つのウェイポイント間を補間
        const FCameraWaypoint& StartWaypoint = Waypoints[CurrentWaypointIndex];
        const FCameraWaypoint& EndWaypoint = Waypoints[FMath::Min(CurrentWaypointIndex + 1, Waypoints.Num() - 1)];

        float LocalProgress = 0.f;
        float ProgressRange = EndWaypoint.Progress - StartWaypoint.Progress;
        if (ProgressRange > 0.f)
        {
            LocalProgress = (Progress - StartWaypoint.Progress) / ProgressRange;
        }

        // 位置と回転を補間
        FVector StartLoc = GetActorTransform().TransformPosition(StartWaypoint.Location);
        FVector EndLoc = GetActorTransform().TransformPosition(EndWaypoint.Location);
        OutLocation = FMath::Lerp(StartLoc, EndLoc, LocalProgress);

        FRotator StartRot = StartWaypoint.Rotation;
        FRotator EndRot = EndWaypoint.Rotation;
        OutRotation = FMath::Lerp(StartRot, EndRot, LocalProgress);

        // FOVも補間
        OutFOV = FMath::Lerp(StartWaypoint.FOV, EndWaypoint.FOV, LocalProgress);

        if (bLookAtPlayer)
        {
            FVector DirectionToPlayer = PlayerLocation - OutLocation;
            OutRotation = DirectionToPlayer.Rotation();
        }
        break;
    }

    case ECameraVolumeType::DollyZoom:
    {
        // ドリーズーム：カメラ位置は固定、FOVを変化させて遠近感を演出
        OutLocation = GetActorTransform().TransformPosition(FixedCameraLocation);

        float DistanceToPlayer = FVector::Dist(OutLocation, PlayerLocation);
        float MinDistance = 500.f;
        float MaxDistance = 3000.f;
        float NormalizedDistance = FMath::GetMappedRangeValueClamped(
            FVector2D(MinDistance, MaxDistance),
            FVector2D(0.f, 1.f),
            DistanceToPlayer
        );

        // 距離に応じてFOVを変化（近いほど広角、遠いほど望遠）
        OutFOV = FMath::Lerp(70.f, 110.f, NormalizedDistance);

        if (bLookAtPlayer)
        {
            FVector DirectionToPlayer = PlayerLocation - OutLocation;
            OutRotation = DirectionToPlayer.Rotation();
        }
        else
        {
            OutRotation = GetActorRotation();
        }
        break;
    }
    }

    // 現在の位置を保存
    LastCameraLocation = OutLocation;
    LastCameraRotation = OutRotation;
}

float ACinematicCameraVolume::GetPlayerProgress(const FVector& PlayerLocation) const
{
    // ボリューム内でのプレイヤーの進行度を計算（0-1）
    FVector LocalPlayerLocation = GetActorTransform().InverseTransformPosition(PlayerLocation);
    FVector BoxExtent = VolumeBox->GetScaledBoxExtent();

    // Y軸方向の進行度を使用（横スクロール想定）
    float Progress = FMath::GetMappedRangeValueClamped(
        FVector2D(-BoxExtent.Y, BoxExtent.Y),
        FVector2D(0.f, 1.f),
        LocalPlayerLocation.Y
    );

    return Progress;
}