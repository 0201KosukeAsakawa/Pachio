/**
 * リトルナイトメア風カメラハンドラー
 * シーンに配置されたCinematicCameraVolumeと連携してカメラを制御
 */

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Camera/CameraComponent.h"
#include "UE5Coro.h"
#include "Kismet/KismetMathLibrary.h"
#include "CameraHandlerComponent.generated.h"

class UCameraComponent;
class ACinematicCameraVolume;


/**
 * カメラ遷移設定
 */
USTRUCT(BlueprintType)
struct FCameraTransitionSettings
{
    GENERATED_BODY()

    /** 遷移速度 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transition")
    float Speed = 3.0f;

    /** イージング関数タイプ */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transition")
    TEnumAsByte<EEasingFunc::Type> EasingType = EEasingFunc::EaseInOut;

    /** ブレンド指数（Easing適用時） */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transition")
    float BlendExp = 2.0f;
};

/**
 * リトルナイトメア風カメラ制御コンポーネント
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PACHIO_API UCameraHandlerComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UCameraHandlerComponent();

    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    /** カメラ初期化 */
    void Init(TObjectPtr<USceneComponent> RootComponent);

    /** カメラコンポーネント取得 */
    UFUNCTION(BlueprintPure, Category = "Camera")
    UCameraComponent* GetCamera() const { return Camera; }

    /** 現在アクティブなカメラボリューム取得 */
    UFUNCTION(BlueprintPure, Category = "Camera")
    ACinematicCameraVolume* GetActiveVolume() const { return ActiveCameraVolume; }

    /** イベント演出中か */
    UFUNCTION(BlueprintPure, Category = "Camera Event")
    bool IsInEventMode() const { return bIsInEventMode; }

    /** 手動でカメラボリュームを設定 */
    UFUNCTION(BlueprintCallable, Category = "Camera")
    void SetActiveCameraVolume(ACinematicCameraVolume* NewVolume, bool bInstantTransition = false);

    /** 近くのカメラボリュームを検索 */
    UFUNCTION(BlueprintCallable, Category = "Camera")
    void FindNearestCameraVolume();

    // ========== イベント演出用 ==========

    /** カスタムイベントカメラ（テスト用） */
    UFUNCTION(BlueprintCallable, Category = "Camera Test")
    void TestEventCamera() { TestEventCameraCoroutine(); }

    UE5Coro::TCoroutine<> TestEventCameraCoroutine();

    /** 特定位置にフォーカス */
    UE5Coro::TCoroutine<> FocusOnLocation(
        FVector EventLocation,
        float FocusDuration = 2.0f,
        float MoveSpeed = 3.0f,
        bool bWaitForComplete = true
    );

    /** 複数地点を順次フォーカス */
    UE5Coro::TCoroutine<> FocusOnMultipleLocations(
        TArray<FVector> Locations,
        float DurationPerLocation = 2.0f,
        float MoveSpeed = 3.0f
    );

    /** カメラズーム演出 */
    UE5Coro::TCoroutine<> ZoomCamera(float TargetFOV, float Duration = 1.0f);

    /** カメラシェイク演出 */
    UE5Coro::TCoroutine<> ShakeCamera(float Intensity = 10.0f, float Duration = 0.5f);

private:
    /** カメラ位置を更新 */
    void UpdateCameraPosition(float DeltaTime);

    /** カメラボリュームに基づいてカメラを更新 */
    void UpdateFromCameraVolume(float DeltaTime);

    /** プレイヤー位置に戻る */
    UE5Coro::TCoroutine<> ReturnToPlayer(float MoveSpeed = 3.0f);

    /** カメラボリュームの優先度を比較 */
    bool ShouldSwitchVolume(ACinematicCameraVolume* NewVolume) const;

private:
    /** カメラコンポーネント */
    UPROPERTY(VisibleAnywhere, Category = "Camera", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UCameraComponent> Camera;

    /** カメラ遷移設定 */
    UPROPERTY(EditAnywhere, Category = "Camera Transition")
    FCameraTransitionSettings TransitionSettings;

    /** カメラボリュームの自動検出を有効にする */
    UPROPERTY(EditAnywhere, Category = "Camera Volume")
    bool bAutoDetectVolumes = true;

    /** カメラボリューム検索範囲 */
    UPROPERTY(EditAnywhere, Category = "Camera Volume", meta = (EditCondition = "bAutoDetectVolumes"))
    float VolumeDetectionRadius = 5000.f;

    /** 現在アクティブなカメラボリューム */
    UPROPERTY()
    TObjectPtr<ACinematicCameraVolume> ActiveCameraVolume;

    /** 検出されたカメラボリューム一覧 */
    UPROPERTY()
    TArray<TObjectPtr<ACinematicCameraVolume>> DetectedVolumes;

    /** 目標カメラ位置 */
    FVector TargetCameraLocation;

    /** 目標カメラ回転 */
    FRotator TargetCameraRotation;

    /** 目標FOV */
    float TargetFOV = 90.f;

    /** 元のFOV */
    float OriginalFOV = 90.f;

    /** イベント演出中フラグ */
    bool bIsInEventMode = false;

    /** イベント前のカメラ位置 */
    FVector PreEventCameraLocation;

    /** イベント前のカメラ回転 */
    FRotator PreEventCameraRotation;

    /** イベント前のFOV */
    float PreEventFOV = 90.f;

    /** 遷移中フラグ */
    bool bIsTransitioning = false;

    /** 遷移開始時刻 */
    float TransitionStartTime = 0.f;

    /** 遷移期間 */
    float TransitionDuration = 0.f;

    /** 遷移開始位置 */
    FVector TransitionStartLocation;

    /** 遷移開始回転 */
    FRotator TransitionStartRotation;

    /** 遷移開始FOV */
    float TransitionStartFOV = 90.f;
};