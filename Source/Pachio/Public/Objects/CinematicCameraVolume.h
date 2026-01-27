/**
 * リトルナイトメア風のシネマティックカメラボリューム
 * レベル内に配置し、プレイヤーが侵入するとカメラ挙動が変化する
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
 * カメラの移動制限設定
 */
USTRUCT(BlueprintType)
struct FCameraConstraints
{
    GENERATED_BODY()
public:
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

    /** プレイヤーに対するオフセット */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Constraints")
    FVector FollowOffset = FVector(-500.f, 0.f, 200.f);

    /** 追従時の遅延（0-1、0=即座、1=遅い） */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Constraints", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float FollowLag = 0.3f;
};

/**
 * レールカメラ用のウェイポイント
 */
USTRUCT(BlueprintType)
struct FCameraWaypoint
{
    GENERATED_BODY()
public:
    /** ウェイポイント位置 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Waypoint")
    FVector Location = FVector::ZeroVector;

    /** カメラの向き */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Waypoint")
    FRotator Rotation = FRotator::ZeroRotator;

    /** このポイントでのFOV */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Waypoint")
    float FOV = 90.f;

    /** このポイントに到達するまでの時間（プレイヤー進行度ベース） */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Waypoint")
    float Progress = 0.f; // 0.0 = 開始, 1.0 = 終了
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
    /** カメラタイプ */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Volume")
    ECameraVolumeType CameraType = ECameraVolumeType::Follow;

    /** ボリュームの範囲 */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera Volume")
    TObjectPtr<UBoxComponent> VolumeBox;

    /** 固定カメラの位置 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fixed Camera", meta = (EditCondition = "CameraType == ECameraVolumeType::Fixed"))
    FVector FixedCameraLocation = FVector::ZeroVector;

    /** 固定カメラの回転 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fixed Camera", meta = (EditCondition = "CameraType == ECameraVolumeType::Fixed"))
    FRotator FixedCameraRotation = FRotator::ZeroRotator;

    /** カメラのFOV */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Volume", meta = (ClampMin = "5.0", ClampMax = "170.0"))
    float CameraFOV = 90.f;

    /** カメラの移動制限 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Follow Camera")
    FCameraConstraints Constraints;

    /** レールカメラのウェイポイント */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rail Camera", meta = (EditCondition = "CameraType == ECameraVolumeType::Rail"))
    TArray<FCameraWaypoint> Waypoints;

    /** カメラ遷移速度 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Volume", meta = (ClampMin = "0.1", ClampMax = "10.0"))
    float TransitionSpeed = 2.0f;

    /** このボリュームの優先度（高いほど優先） */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Volume")
    int32 Priority = 0;

    /** プレイヤーを見続けるか */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Volume")
    bool bLookAtPlayer = true;

    /** デバッグ表示 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
    bool bShowDebug = true;

private:
    /** 現在アクティブか */
    bool bIsActive = false;

    /** 前回のカメラ位置（補間用） */
    FVector LastCameraLocation;

    /** 前回のカメラ回転（補間用） */
    FRotator LastCameraRotation;
};