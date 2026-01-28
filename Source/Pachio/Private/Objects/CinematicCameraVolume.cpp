/**
 * CinematicCameraVolume.cpp
 * シネマティックカメラボリュームの実装 - 位置固定問題修正版
 */

#include "Objects/CinematicCameraVolume.h"
#include "Components/CameraHandlerComponent.h"
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

    // デフォルト値設定（すべてワールド座標）
    FixedCameraLocation = FVector(-2000.f, 0.f, 500.f);
    FollowOffset = FVector(-2000.f, 0.f, 500.f);
    FollowLag = 0.3f;
    bAllowMoveX = false;
    bAllowMoveY = true;
    bAllowMoveZ = false;
    FollowMinBounds = FVector(-2000.f, -10000.f, 500.f);
    FollowMaxBounds = FVector(-2000.f, 10000.f, 500.f);
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

    // フラット化されたプロパティから構造体に値をコピー
    Constraints.bAllowMoveX = bAllowMoveX;
    Constraints.bAllowMoveY = bAllowMoveY;
    Constraints.bAllowMoveZ = bAllowMoveZ;
    Constraints.FollowOffset = FollowOffset;
    Constraints.FollowLag = FollowLag;
    Constraints.MinBounds = FollowMinBounds;
    Constraints.MaxBounds = FollowMaxBounds;

    CameraAngleLimits.bEnableLimits = bEnableAngleLimits;
    CameraAngleLimits.MinPitch = MinPitch;
    CameraAngleLimits.MaxPitch = MaxPitch;
    CameraAngleLimits.MinYaw = MinYaw;
    CameraAngleLimits.MaxYaw = MaxYaw;
    CameraAngleLimits.LookAtOffset = LookAtOffset;

    // ★デバッグログ：設定値を確認
    if (CameraType == ECameraVolumeType::Fixed || CameraType == ECameraVolumeType::DollyZoom)
    {
        UE_LOG(LogTemp, Log, TEXT("[%s] Fixed Camera Location: %s"), *GetName(), *FixedCameraLocation.ToString());
    }
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

        // 固定カメラ位置の表示（ワールド座標）
        if (CameraType == ECameraVolumeType::Fixed || CameraType == ECameraVolumeType::DollyZoom)
        {
            // ★重要：FixedCameraLocationが(0,0,0)かチェック
            if (FixedCameraLocation.IsNearlyZero())
            {
                DrawDebugString(GetWorld(), GetActorLocation() + FVector(0, 0, 200.f),
                    TEXT("WARNING: Fixed Camera Location is (0,0,0)!"),
                    nullptr, FColor::Red, 0.f, true, 2.0f);
            }

            DrawDebugSphere(GetWorld(), FixedCameraLocation, 80.f, 8, FColor::Red, false, -1.f, 0, 3.f);
            DrawDebugLine(GetWorld(), FixedCameraLocation, GetActorLocation(), FColor::Red, false, -1.f, 0, 2.f);
            DrawDebugString(GetWorld(), FixedCameraLocation + FVector(0, 0, 100),
                FString::Printf(TEXT("Camera: %s"), *FixedCameraLocation.ToString()),
                nullptr, FColor::Red, 0.f, true);
        }

        // 追従ラインの表示（ConstrainedFollow - ワールド座標）
        if (CameraType == ECameraVolumeType::ConstrainedFollow)
        {
            if (bAllowMoveY)
            {
                FVector LineStart = FVector(FollowMinBounds.X, FollowMinBounds.Y, FollowMinBounds.Z);
                FVector LineEnd = FVector(FollowMaxBounds.X, FollowMaxBounds.Y, FollowMinBounds.Z);
                DrawDebugLine(GetWorld(), LineStart, LineEnd, FColor::Magenta, false, -1.f, 0, 5.f);

                DrawDebugSphere(GetWorld(), LineStart, 50.f, 8, FColor::Magenta, false, -1.f, 0, 3.f);
                DrawDebugSphere(GetWorld(), LineEnd, 50.f, 8, FColor::Magenta, false, -1.f, 0, 3.f);

                DrawDebugString(GetWorld(), LineStart + FVector(0, 0, 100),
                    FString::Printf(TEXT("Y Min: %.0f"), FollowMinBounds.Y),
                    nullptr, FColor::Magenta, 0.f, true);
                DrawDebugString(GetWorld(), LineEnd + FVector(0, 0, 100),
                    FString::Printf(TEXT("Y Max: %.0f"), FollowMaxBounds.Y),
                    nullptr, FColor::Magenta, 0.f, true);
            }

            if (bAllowMoveZ)
            {
                FVector LineStart = FVector(FollowMinBounds.X, FollowMinBounds.Y, FollowMinBounds.Z);
                FVector LineEnd = FVector(FollowMinBounds.X, FollowMinBounds.Y, FollowMaxBounds.Z);
                DrawDebugLine(GetWorld(), LineStart, LineEnd, FColor::Cyan, false, -1.f, 0, 5.f);

                DrawDebugSphere(GetWorld(), LineStart, 50.f, 8, FColor::Cyan, false, -1.f, 0, 3.f);
                DrawDebugSphere(GetWorld(), LineEnd, 50.f, 8, FColor::Cyan, false, -1.f, 0, 3.f);

                DrawDebugString(GetWorld(), LineStart + FVector(100, 0, 0),
                    FString::Printf(TEXT("Z Min: %.0f"), FollowMinBounds.Z),
                    nullptr, FColor::Cyan, 0.f, true);
                DrawDebugString(GetWorld(), LineEnd + FVector(100, 0, 0),
                    FString::Printf(TEXT("Z Max: %.0f"), FollowMaxBounds.Z),
                    nullptr, FColor::Cyan, 0.f, true);
            }

            if (bAllowMoveX)
            {
                FVector LineStart = FVector(FollowMinBounds.X, FollowMinBounds.Y, FollowMinBounds.Z);
                FVector LineEnd = FVector(FollowMaxBounds.X, FollowMinBounds.Y, FollowMinBounds.Z);
                DrawDebugLine(GetWorld(), LineStart, LineEnd, FColor::Orange, false, -1.f, 0, 5.f);

                DrawDebugSphere(GetWorld(), LineStart, 50.f, 8, FColor::Orange, false, -1.f, 0, 3.f);
                DrawDebugSphere(GetWorld(), LineEnd, 50.f, 8, FColor::Orange, false, -1.f, 0, 3.f);

                DrawDebugString(GetWorld(), LineStart + FVector(0, 100, 0),
                    FString::Printf(TEXT("X Min: %.0f"), FollowMinBounds.X),
                    nullptr, FColor::Orange, 0.f, true);
                DrawDebugString(GetWorld(), LineEnd + FVector(0, 100, 0),
                    FString::Printf(TEXT("X Max: %.0f"), FollowMaxBounds.X),
                    nullptr, FColor::Orange, 0.f, true);
            }
        }

        // ウェイポイント表示（レールカメラ - ワールド座標）
        if (CameraType == ECameraVolumeType::Rail)
        {
            for (int32 i = 0; i < Waypoints.Num(); ++i)
            {
                FVector WaypointWorld = Waypoints[i].Location;
                DrawDebugSphere(GetWorld(), WaypointWorld, 80.f, 12, FColor::Cyan, false, -1.f, 0, 3.f);
                DrawDebugString(GetWorld(), WaypointWorld + FVector(0, 0, 150),
                    FString::Printf(TEXT("WP%d (%.2f)"), i, Waypoints[i].Progress),
                    nullptr, FColor::Cyan, 0.f, true);

                if (i < Waypoints.Num() - 1)
                {
                    FVector NextWaypointWorld = Waypoints[i + 1].Location;
                    DrawDebugLine(GetWorld(), WaypointWorld, NextWaypointWorld, FColor::Cyan, false, -1.f, 0, 3.f);
                }
            }
        }
    }
}

void ACinematicCameraVolume::OnPlayerEnter(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    ACharacter* Character = Cast<ACharacter>(OtherActor);
    if (!Character)
        return;

    bIsActive = true;
    UE_LOG(LogTemp, Log, TEXT("Player entered camera volume: %s (Priority: %d)"), *GetName(), Priority);

    UCameraHandlerComponent* CameraHandler = Character->GetComponentByClass<UCameraHandlerComponent>();
    if (CameraHandler)
    {
        CameraHandler->SetActiveCameraVolume(this, false);
        UE_LOG(LogTemp, Log, TEXT("Camera volume activated: %s"), *GetName());
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("CameraHandlerComponent not found on character!"));
    }
}

void ACinematicCameraVolume::OnPlayerExit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    ACharacter* Character = Cast<ACharacter>(OtherActor);
    if (!Character)
        return;

    bIsActive = false;
    UE_LOG(LogTemp, Log, TEXT("Player exited camera volume: %s"), *GetName());

    UCameraHandlerComponent* CameraHandler = Character->GetComponentByClass<UCameraHandlerComponent>();
    if (CameraHandler && CameraHandler->GetActiveVolume() == this)
    {
        CameraHandler->FindNearestCameraVolume();
    }
}

void ACinematicCameraVolume::GetCameraTransform(const FVector& PlayerLocation, FVector& OutLocation, FRotator& OutRotation, float& OutFOV)
{
    OutFOV = CameraFOV;

    switch (CameraType)
    {
    case ECameraVolumeType::Fixed:
    {
        // ★修正：ワールド座標をそのまま使用
        OutLocation = FixedCameraLocation;
        OutRotation = FixedCameraRotation;

        // ★デバッグログ
        UE_LOG(LogTemp, VeryVerbose, TEXT("[%s] Fixed Camera - OutLocation: %s"), *GetName(), *OutLocation.ToString());

        if (bLookAtPlayer)
        {
            FVector LookAtTarget = PlayerLocation + LookAtOffset;
            FVector DirectionToPlayer = LookAtTarget - OutLocation;
            OutRotation = DirectionToPlayer.Rotation();

            if (bEnableAngleLimits)
            {
                OutRotation.Pitch = FMath::Clamp(OutRotation.Pitch, MinPitch, MaxPitch);
                OutRotation.Yaw = FMath::Clamp(OutRotation.Yaw, MinYaw, MaxYaw);
            }
        }
        break;
    }

    case ECameraVolumeType::Follow:
    case ECameraVolumeType::ConstrainedFollow:
    {
        // 追従カメラ：プレイヤー位置 + オフセット
        FVector TargetLocation = PlayerLocation + FollowOffset;

        if (CameraType == ECameraVolumeType::ConstrainedFollow)
        {
            // ★修正：軸が許可されていない場合、オフセットを絶対位置として使用
            FVector BaseLocation = PlayerLocation;

            if (!bAllowMoveX)
            {
                // X軸追従しない = FollowOffset.Xを絶対位置として使用
                TargetLocation.X = FollowOffset.X;
            }
            if (!bAllowMoveY)
            {
                // Y軸追従しない = FollowOffset.Yを絶対位置として使用
                TargetLocation.Y = FollowOffset.Y;
            }
            if (!bAllowMoveZ)
            {
                // Z軸追従しない = FollowOffset.Zを絶対位置として使用
                TargetLocation.Z = FollowOffset.Z;
            }

            // 範囲制限（ワールド座標）
            TargetLocation.X = FMath::Clamp(TargetLocation.X, FollowMinBounds.X, FollowMaxBounds.X);
            TargetLocation.Y = FMath::Clamp(TargetLocation.Y, FollowMinBounds.Y, FollowMaxBounds.Y);
            TargetLocation.Z = FMath::Clamp(TargetLocation.Z, FollowMinBounds.Z, FollowMaxBounds.Z);
        }

        // 遅延追従
        float LagSpeed = FMath::GetMappedRangeValueClamped(FVector2D(0.f, 1.f), FVector2D(10.f, 1.f), FollowLag);
        OutLocation = FMath::VInterpTo(LastCameraLocation, TargetLocation, GetWorld()->GetDeltaSeconds(), LagSpeed);

        if (bLookAtPlayer)
        {
            FVector LookAtTarget = PlayerLocation + LookAtOffset;
            FVector DirectionToPlayer = LookAtTarget - OutLocation;
            OutRotation = DirectionToPlayer.Rotation();

            if (bEnableAngleLimits)
            {
                OutRotation.Pitch = FMath::Clamp(OutRotation.Pitch, MinPitch, MaxPitch);
                OutRotation.Yaw = FMath::Clamp(OutRotation.Yaw, MinYaw, MaxYaw);
            }
        }
        else
        {
            OutRotation = GetActorRotation();
        }
        break;
    }

    case ECameraVolumeType::Rail:
    {
        if (Waypoints.Num() < 2)
        {
            // ★フォールバック：ウェイポイントが不足している場合
            OutLocation = GetActorLocation();
            OutRotation = GetActorRotation();
            UE_LOG(LogTemp, Warning, TEXT("[%s] Rail Camera needs at least 2 waypoints!"), *GetName());
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

        const FCameraWaypoint& StartWaypoint = Waypoints[CurrentWaypointIndex];
        const FCameraWaypoint& EndWaypoint = Waypoints[FMath::Min(CurrentWaypointIndex + 1, Waypoints.Num() - 1)];

        float LocalProgress = 0.f;
        float ProgressRange = EndWaypoint.Progress - StartWaypoint.Progress;
        if (ProgressRange > 0.f)
        {
            LocalProgress = (Progress - StartWaypoint.Progress) / ProgressRange;
        }

        // ★ワールド座標をそのまま補間
        OutLocation = FMath::Lerp(StartWaypoint.Location, EndWaypoint.Location, LocalProgress);
        OutRotation = FMath::Lerp(StartWaypoint.Rotation, EndWaypoint.Rotation, LocalProgress);
        OutFOV = FMath::Lerp(StartWaypoint.FOV, EndWaypoint.FOV, LocalProgress);

        if (bLookAtPlayer)
        {
            FVector LookAtTarget = PlayerLocation + LookAtOffset;
            FVector DirectionToPlayer = LookAtTarget - OutLocation;
            OutRotation = DirectionToPlayer.Rotation();
        }
        break;
    }

    case ECameraVolumeType::DollyZoom:
    {
        // ★修正：ワールド座標をそのまま使用
        OutLocation = FixedCameraLocation;

        float DistanceToPlayer = FVector::Dist(OutLocation, PlayerLocation);
        float MinDistance = 500.f;
        float MaxDistance = 3000.f;
        float NormalizedDistance = FMath::GetMappedRangeValueClamped(
            FVector2D(MinDistance, MaxDistance),
            FVector2D(0.f, 1.f),
            DistanceToPlayer
        );

        OutFOV = FMath::Lerp(70.f, 110.f, NormalizedDistance);

        if (bLookAtPlayer)
        {
            FVector LookAtTarget = PlayerLocation + LookAtOffset;
            FVector DirectionToPlayer = LookAtTarget - OutLocation;
            OutRotation = DirectionToPlayer.Rotation();

            if (bEnableAngleLimits)
            {
                OutRotation.Pitch = FMath::Clamp(OutRotation.Pitch, MinPitch, MaxPitch);
                OutRotation.Yaw = FMath::Clamp(OutRotation.Yaw, MinYaw, MaxYaw);
            }
        }
        else
        {
            OutRotation = FixedCameraRotation;
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
    FVector LocalPlayerLocation = GetActorTransform().InverseTransformPosition(PlayerLocation);
    FVector BoxExtent = VolumeBox->GetScaledBoxExtent();

    float Progress = FMath::GetMappedRangeValueClamped(
        FVector2D(-BoxExtent.Y, BoxExtent.Y),
        FVector2D(0.f, 1.f),
        LocalPlayerLocation.Y
    );

    return Progress;
}