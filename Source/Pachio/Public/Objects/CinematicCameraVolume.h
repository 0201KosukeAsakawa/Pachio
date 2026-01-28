/**
 * CinematicCameraVolume.h
 * リトルナイトメア風のシネマティックカメラボリューム - ワールド座標版
 * すべての座標設定をワールド座標で統一
 */

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "CinematicCameraVolume.generated.h"

 /**
  * カメラの挙動タイプ
  */
UENUM(BlueprintType)
enum class ECameraVolumeType : uint8
{
    /** 固定カメラ（位置・回転固定） */
    Fixed UMETA(DisplayName = "Fixed Camera"),

    /** 追従カメラ（プレイヤーを追従、角度固定） */
    Follow UMETA(DisplayName = "Follow Camera"),

    /** レールカメラ（スプライン上を移動） */
    Rail UMETA(DisplayName = "Rail Camera"),

    /** 制限付き追従（特定軸のみ追従） */
    ConstrainedFollow UMETA(DisplayName = "Constrained Follow"),

    /** ドリーズーム（位置固定、FOV変化） */
    DollyZoom UMETA(DisplayName = "Dolly Zoom")
};

/**
 * カメラの角度制限設定
 */
USTRUCT(BlueprintType)
struct FCameraAngleLimits
{
    GENERATED_BODY()

    /** 角度制限を有効にする */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Angle Limits")
    bool bEnableLimits = false;

    /** Pitch（上下角度）の最小値 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Angle Limits", meta = (EditCondition = "bEnableLimits", ClampMin = "-90.0", ClampMax = "90.0"))
    float MinPitch = -30.f;

    /** Pitch（上下角度）の最大値 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Angle Limits", meta = (EditCondition = "bEnableLimits", ClampMin = "-90.0", ClampMax = "90.0"))
    float MaxPitch = 30.f;

    /** Yaw（左右角度）の最小値 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Angle Limits", meta = (EditCondition = "bEnableLimits", ClampMin = "-180.0", ClampMax = "180.0"))
    float MinYaw = -45.f;

    /** Yaw（左右角度）の最大値 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Angle Limits", meta = (EditCondition = "bEnableLimits", ClampMin = "-180.0", ClampMax = "180.0"))
    float MaxYaw = 45.f;

    /** プレイヤーを見る際の注視点オフセット（プレイヤーの頭や中心を見る） */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Angle Limits", meta = (EditCondition = "bEnableLimits"))
    FVector LookAtOffset = FVector(0.f, 0.f, 100.f);
};

/**
 * カメラの移動制限設定
 */
USTRUCT(BlueprintType)
struct FCameraConstraints
{
    GENERATED_BODY()

    /** X軸の移動を許可 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Constraints")
    bool bAllowMoveX = false;

    /** Y軸の移動を許可 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Constraints")
    bool bAllowMoveY = true;

    /** Z軸の移動を許可 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Constraints")
    bool bAllowMoveZ = true;

    /** 移動の最小値 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Constraints")
    FVector MinBounds = FVector(-10000.f);

    /** 移動の最大値 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Constraints")
    FVector MaxBounds = FVector(10000.f);

    /** プレイヤーに対するオフセット（カメラ距離と位置） */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Constraints")
    FVector FollowOffset = FVector(-500.f, 0.f, 200.f);

    /** 追従時の遅延（0-1、0=即座、1=遅い） */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Constraints", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float FollowLag = 0.3f;
};

/**
 * レールカメラ用のウェイポイント（ワールド座標）
 */
USTRUCT(BlueprintType)
struct FCameraWaypoint
{
    GENERATED_BODY()

    /** ウェイポイント位置（ワールド座標） */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Waypoint", meta = (MakeEditWidget = true))
    FVector Location = FVector::ZeroVector;

    /** カメラの向き */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Waypoint")
    FRotator Rotation = FRotator::ZeroRotator;

    /** このポイントでのFOV */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Waypoint")
    float FOV = 90.f;

    /** このポイントに到達するまでの進行度（0.0 = 開始, 1.0 = 終了） */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Waypoint")
    float Progress = 0.f;
};

/**
 * シネマティックカメラボリューム
 * プレイヤーがこのボリュームに入ると、指定されたカメラ挙動に切り替わる
 */
UCLASS()
class PACHIO_API ACinematicCameraVolume : public AActor
{
    GENERATED_BODY()

public:
    ACinematicCameraVolume();

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    /** プレイヤーがボリュームに侵入 */
    UFUNCTION()
    void OnPlayerEnter(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
        const FHitResult& SweepResult);

    /** プレイヤーがボリュームから退出 */
    UFUNCTION()
    void OnPlayerExit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

    /** 現在のカメラ位置と回転を計算 */
    void GetCameraTransform(const FVector& PlayerLocation, FVector& OutLocation, FRotator& OutRotation, float& OutFOV);

    /** プレイヤーの進行度を取得（0-1） */
    float GetPlayerProgress(const FVector& PlayerLocation) const;

public:
    // ========== 基本設定 ==========

    /** カメラタイプ */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "1. Camera Type")
    ECameraVolumeType CameraType = ECameraVolumeType::Follow;

    /** ボリュームの範囲 */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "1. Camera Type")
    TObjectPtr<UBoxComponent> VolumeBox;

    /** このボリュームの優先度（高いほど優先、複数重なった時に使用） */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "1. Camera Type", meta = (ClampMin = "0", ClampMax = "100"))
    int32 Priority = 10;

    // ========== 固定カメラ設定（Fixed / DollyZoom） ==========

    /** 固定カメラの位置（ワールド座標） */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "2. Fixed Camera Position",
        meta = (EditCondition = "CameraType == ECameraVolumeType::Fixed || CameraType == ECameraVolumeType::DollyZoom", EditConditionHides,
            MakeEditWidget = true))
    FVector FixedCameraLocation = FVector(-2000.f, 0.f, 500.f);

    /** 固定カメラの回転（Look At Playerがfalseの時のみ有効） */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "2. Fixed Camera Position",
        meta = (EditCondition = "CameraType == ECameraVolumeType::Fixed && !bLookAtPlayer", EditConditionHides))
    FRotator FixedCameraRotation = FRotator::ZeroRotator;

    // ========== 追従カメラ設定（Follow / ConstrainedFollow） ==========

    /** プレイヤーからのカメラオフセット（X:後ろ距離, Y:横, Z:高さ） */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "3. Follow Camera Settings",
        meta = (EditCondition = "CameraType == ECameraVolumeType::Follow || CameraType == ECameraVolumeType::ConstrainedFollow", EditConditionHides))
    FVector FollowOffset = FVector(-2000.f, 0.f, 500.f);

    /** 追従の遅延（0=即座、1=遅い） */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "3. Follow Camera Settings",
        meta = (EditCondition = "CameraType == ECameraVolumeType::Follow || CameraType == ECameraVolumeType::ConstrainedFollow", EditConditionHides,
            ClampMin = "0.0", ClampMax = "1.0"))
    float FollowLag = 0.3f;

    // ========== 制限付き追従設定（ConstrainedFollow） ==========

    /** X軸の追従を許可（通常はfalse：カメラは固定距離を保つ） */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "4. Constrained Follow Settings",
        meta = (EditCondition = "CameraType == ECameraVolumeType::ConstrainedFollow", EditConditionHides))
    bool bAllowMoveX = false;

    /** Y軸の追従を許可（横スクロールならtrue） */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "4. Constrained Follow Settings",
        meta = (EditCondition = "CameraType == ECameraVolumeType::ConstrainedFollow", EditConditionHides))
    bool bAllowMoveY = true;

    /** Z軸の追従を許可（縦スクロールならtrue） */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "4. Constrained Follow Settings",
        meta = (EditCondition = "CameraType == ECameraVolumeType::ConstrainedFollow", EditConditionHides))
    bool bAllowMoveZ = false;

    /** 追従の最小範囲（ワールド座標） */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "4. Constrained Follow Settings",
        meta = (EditCondition = "CameraType == ECameraVolumeType::ConstrainedFollow", EditConditionHides,
            MakeEditWidget = true,
            Tooltip = "カメラが移動できる最小位置（ワールド座標：例：Y軸なら開始地点）"))
    FVector FollowMinBounds = FVector(-2000.f, -10000.f, 500.f);

    /** 追従の最大範囲（ワールド座標） */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "4. Constrained Follow Settings",
        meta = (EditCondition = "CameraType == ECameraVolumeType::ConstrainedFollow", EditConditionHides,
            MakeEditWidget = true,
            Tooltip = "カメラが移動できる最大位置（ワールド座標：例：Y軸なら終了地点）"))
    FVector FollowMaxBounds = FVector(-2000.f, 10000.f, 500.f);

    // ========== レールカメラ設定（Rail） ==========

    /** レールカメラのウェイポイント（すべてワールド座標） */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "5. Rail Camera Settings",
        meta = (EditCondition = "CameraType == ECameraVolumeType::Rail", EditConditionHides,
            Tooltip = "プレイヤーの進行度（0.0～1.0）に応じてカメラが移動する経路（ワールド座標）"))
    TArray<FCameraWaypoint> Waypoints;

    // ========== カメラ角度設定（Look At Playerがtrueの時） ==========

    /** プレイヤーを見続けるか */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "6. Camera Angle Settings")
    bool bLookAtPlayer = true;

    /** カメラ角度制限を有効にする */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "6. Camera Angle Settings",
        meta = (EditCondition = "bLookAtPlayer", EditConditionHides))
    bool bEnableAngleLimits = false;

    /** Pitch（上下角度）の最小値 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "6. Camera Angle Settings",
        meta = (EditCondition = "bLookAtPlayer && bEnableAngleLimits", EditConditionHides,
            ClampMin = "-90.0", ClampMax = "90.0",
            Tooltip = "カメラが下を向ける角度の限界（例：-30なら下30度まで）"))
    float MinPitch = -30.f;

    /** Pitch（上下角度）の最大値 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "6. Camera Angle Settings",
        meta = (EditCondition = "bLookAtPlayer && bEnableAngleLimits", EditConditionHides,
            ClampMin = "-90.0", ClampMax = "90.0",
            Tooltip = "カメラが上を向ける角度の限界（例：10なら上10度まで）"))
    float MaxPitch = 30.f;

    /** Yaw（左右角度）の最小値 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "6. Camera Angle Settings",
        meta = (EditCondition = "bLookAtPlayer && bEnableAngleLimits", EditConditionHides,
            ClampMin = "-180.0", ClampMax = "180.0",
            Tooltip = "カメラが左を向ける角度の限界（例：-45なら左45度まで）"))
    float MinYaw = -45.f;

    /** Yaw（左右角度）の最大値 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "6. Camera Angle Settings",
        meta = (EditCondition = "bLookAtPlayer && bEnableAngleLimits", EditConditionHides,
            ClampMin = "-180.0", ClampMax = "180.0",
            Tooltip = "カメラが右を向ける角度の限界（例：45なら右45度まで）"))
    float MaxYaw = 45.f;

    /** プレイヤーのどこを見るか（頭、中心など） */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "6. Camera Angle Settings",
        meta = (EditCondition = "bLookAtPlayer", EditConditionHides,
            Tooltip = "プレイヤー位置からのオフセット（例：Z=100なら頭の高さ）"))
    FVector LookAtOffset = FVector(0.f, 0.f, 100.f);

    // ========== その他のカメラ設定 ==========

    /** カメラのFOV（視野角） */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "7. Other Settings",
        meta = (ClampMin = "5.0", ClampMax = "170.0",
            Tooltip = "小さい値=望遠（70）、標準（90）、大きい値=広角（110）"))
    float CameraFOV = 90.f;

    /** カメラ遷移速度 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "7. Other Settings",
        meta = (ClampMin = "0.1", ClampMax = "10.0",
            Tooltip = "別のボリュームから切り替わる速度"))
    float TransitionSpeed = 2.0f;

    /** デバッグ表示 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "8. Debug")
    bool bShowDebug = true;

private:
    /** 現在アクティブか */
    bool bIsActive = false;

    /** 前回のカメラ位置（補間用） */
    FVector LastCameraLocation;

    /** 前回のカメラ回転（補間用） */
    FRotator LastCameraRotation;

    // 内部用：構造体からフラット化
    UPROPERTY()
    FCameraConstraints Constraints;

    UPROPERTY()
    FCameraAngleLimits CameraAngleLimits;
};