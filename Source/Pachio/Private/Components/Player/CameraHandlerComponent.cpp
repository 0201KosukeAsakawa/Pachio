#include "Components/CameraHandlerComponent.h"
#include "Camera/CameraComponent.h"
#include "UE5Coro.h"
#include "Objects/CinematicCameraVolume.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "EngineUtils.h"

UCameraHandlerComponent::UCameraHandlerComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
}

void UCameraHandlerComponent::BeginPlay()
{
    Super::BeginPlay();

    if (Camera)
    {
        OriginalFOV = Camera->FieldOfView;
        TargetFOV = OriginalFOV;
    }

    // カメラボリュームを自動検出
    if (bAutoDetectVolumes)
    {
        FindNearestCameraVolume();
    }
}

void UCameraHandlerComponent::Init(TObjectPtr<USceneComponent> RootComponent)
{
    if (!Camera)
        return;

    //// カメラをルートにアタッチ
    //if (RootComponent && !Camera->GetAttachParent())
    //{
    //    Camera->SetupAttachment(RootComponent);
    //    Camera->RegisterComponent();
    //}

    if (Camera)
    {
        OriginalFOV = Camera->FieldOfView;
        TargetCameraLocation = Camera->GetComponentLocation();
        TargetCameraRotation = Camera->GetComponentRotation();
    }
}

void UCameraHandlerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (bIsInEventMode)
    {
        // イベント演出中は処理をスキップ
        return;
    }

    // カメラボリュームベースの更新
    if (ActiveCameraVolume)
    {
        UpdateFromCameraVolume(DeltaTime);
    }
    else
    {
        // カメラボリュームがない場合、定期的に検索
        if (bAutoDetectVolumes)
        {
            static float SearchTimer = 0.f;
            SearchTimer += DeltaTime;
            if (SearchTimer > 1.0f) // 1秒ごとに検索
            {
                FindNearestCameraVolume();
                SearchTimer = 0.f;
            }
        }
    }

    UpdateCameraPosition(DeltaTime);
}

void UCameraHandlerComponent::UpdateFromCameraVolume(float DeltaTime)
{
    if (!ActiveCameraVolume || !GetOwner())
        return;

    FVector PlayerLocation = GetOwner()->GetActorLocation();
    FVector NewLocation;
    FRotator NewRotation;
    float NewFOV;

    // カメラボリュームから目標値を取得
    ActiveCameraVolume->GetCameraTransform(PlayerLocation, NewLocation, NewRotation, NewFOV);

    TargetCameraLocation = NewLocation;
    TargetCameraRotation = NewRotation;
    TargetFOV = NewFOV;
}

void UCameraHandlerComponent::UpdateCameraPosition(float DeltaTime)
{
    if (!Camera)
        return;

    // 遷移中の処理
    if (bIsTransitioning)
    {
        float ElapsedTime = GetWorld()->GetTimeSeconds() - TransitionStartTime;
        float Alpha = FMath::Clamp(ElapsedTime / TransitionDuration, 0.f, 1.f);

        // イージング適用
        Alpha = UKismetMathLibrary::Ease(0.f, 1.f, Alpha, TransitionSettings.EasingType, TransitionSettings.BlendExp);

        // 位置・回転・FOVを補間
        FVector CurrentLocation = FMath::Lerp(TransitionStartLocation, TargetCameraLocation, Alpha);
        FRotator CurrentRotation = FMath::Lerp(TransitionStartRotation, TargetCameraRotation, Alpha);
        float CurrentFOV = FMath::Lerp(TransitionStartFOV, TargetFOV, Alpha);

        Camera->SetWorldLocation(CurrentLocation);
        Camera->SetWorldRotation(CurrentRotation);
        Camera->SetFieldOfView(CurrentFOV);

        if (Alpha >= 1.0f)
        {
            bIsTransitioning = false;
        }
    }
    else
    {
        // 通常の補間
        FVector CurrentLocation = Camera->GetComponentLocation();
        FRotator CurrentRotation = Camera->GetComponentRotation();
        float CurrentFOV = Camera->FieldOfView;

        FVector NewLocation = FMath::VInterpTo(CurrentLocation, TargetCameraLocation, DeltaTime, TransitionSettings.Speed);
        FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetCameraRotation, DeltaTime, TransitionSettings.Speed);
        float NewFOV = FMath::FInterpTo(CurrentFOV, TargetFOV, DeltaTime, TransitionSettings.Speed);

        Camera->SetWorldLocation(NewLocation);
        Camera->SetWorldRotation(NewRotation);
        Camera->SetFieldOfView(NewFOV);
    }
}

void UCameraHandlerComponent::SetActiveCameraVolume(ACinematicCameraVolume* NewVolume, bool bInstantTransition)
{
    if (NewVolume == ActiveCameraVolume)
        return;

    // 優先度チェック
    if (ActiveCameraVolume && !ShouldSwitchVolume(NewVolume))
        return;

    UE_LOG(LogTemp, Log, TEXT("Switching to camera volume: %s"), NewVolume ? *NewVolume->GetName() : TEXT("None"));

    ActiveCameraVolume = NewVolume;

    if (bInstantTransition)
    {
        // 即座に切り替え
        if (NewVolume && GetOwner())
        {
            FVector PlayerLocation = GetOwner()->GetActorLocation();
            NewVolume->GetCameraTransform(PlayerLocation, TargetCameraLocation, TargetCameraRotation, TargetFOV);

            if (Camera)
            {
                Camera->SetWorldLocation(TargetCameraLocation);
                Camera->SetWorldRotation(TargetCameraRotation);
                Camera->SetFieldOfView(TargetFOV);
            }
        }
    }
    else
    {
        // スムーズに遷移
        bIsTransitioning = true;
        TransitionStartTime = GetWorld()->GetTimeSeconds();
        TransitionDuration = 1.0f / TransitionSettings.Speed;

        if (Camera)
        {
            TransitionStartLocation = Camera->GetComponentLocation();
            TransitionStartRotation = Camera->GetComponentRotation();
            TransitionStartFOV = Camera->FieldOfView;
        }
    }
}

void UCameraHandlerComponent::FindNearestCameraVolume()
{
    if (!GetOwner())
        return;

    DetectedVolumes.Empty();

    // ワールド内の全カメラボリュームを取得
    for (TActorIterator<ACinematicCameraVolume> It(GetWorld()); It; ++It)
    {
        ACinematicCameraVolume* Volume = *It;
        if (!Volume)
            continue;

        float Distance = FVector::Dist(GetOwner()->GetActorLocation(), Volume->GetActorLocation());
        if (Distance <= VolumeDetectionRadius)
        {
            DetectedVolumes.Add(Volume);
        }
    }

    // 優先度でソート（高い順）
    DetectedVolumes.Sort([](const TObjectPtr<ACinematicCameraVolume>& A, const TObjectPtr<ACinematicCameraVolume>& B)
        {
            return A->Priority > B->Priority;
        });

    // 最も優先度の高いボリュームを設定
    if (DetectedVolumes.Num() > 0)
    {
        SetActiveCameraVolume(DetectedVolumes[0], false);
    }
}

bool UCameraHandlerComponent::ShouldSwitchVolume(ACinematicCameraVolume* NewVolume) const
{
    if (!NewVolume)
        return false;

    if (!ActiveCameraVolume)
        return true;

    // 優先度が高い場合のみ切り替え
    return NewVolume->Priority >= ActiveCameraVolume->Priority;
}

// ========== イベント演出コルーチン ==========

UE5Coro::TCoroutine<> UCameraHandlerComponent::TestEventCameraCoroutine()
{
    using namespace UE5Coro;

    if (!GetOwner() || !Camera)
        co_return;

    UWorld* World = GetWorld();
    if (!World)
        co_return;

    // イベント開始
    bIsInEventMode = true;
    PreEventCameraLocation = Camera->GetComponentLocation();
    PreEventCameraRotation = Camera->GetComponentRotation();
    PreEventFOV = Camera->FieldOfView;

    // Time Dilationで演出
    UGameplayStatics::SetGlobalTimeDilation(World, 0.01f);

    // リアルタイムで1秒待機
    co_await Latent::Seconds(1.0f);

    // カメラ移動先
    FVector TestLocation = GetOwner()->GetActorLocation() + FVector(-500.f, 1500.f, 800.f);

    // フォーカス
    co_await FocusOnLocation(TestLocation, 2.0f, 3.0f, true);

    // 時間を元に戻す
    UGameplayStatics::SetGlobalTimeDilation(World, 1.0f);

    bIsInEventMode = false;

    co_return;
}

UE5Coro::TCoroutine<> UCameraHandlerComponent::FocusOnLocation(
    FVector EventLocation,
    float FocusDuration,
    float MoveSpeed,
    bool bWaitForComplete)
{
    using namespace UE5Coro;

    if (!Camera || !GetOwner())
        co_return;

    bIsInEventMode = true;

    FVector TargetLocation = EventLocation;
    const float StopThreshold = 50.0f;
    const float MaxDuration = 10.0f;
    float ElapsedTime = 0.0f;

    if (bWaitForComplete)
    {
        // 移動完了まで待つ
        while (FVector::Dist(Camera->GetComponentLocation(), TargetLocation) > StopThreshold)
        {
            ElapsedTime += GetWorld()->GetDeltaSeconds();
            if (ElapsedTime > MaxDuration)
            {
                UE_LOG(LogTemp, Warning, TEXT("FocusOnLocation: Timeout"));
                break;
            }

            FVector CurrentLocation = Camera->GetComponentLocation();
            FVector NewLocation = FMath::VInterpTo(CurrentLocation, TargetLocation,
                GetWorld()->GetDeltaSeconds(), MoveSpeed);
            Camera->SetWorldLocation(NewLocation);

            co_await Latent::NextTick();
        }

        // 最終位置を確実に設定
        Camera->SetWorldLocation(TargetLocation);
    }
    else
    {
        Camera->SetWorldLocation(TargetLocation);
    }

    // 注視時間待機
    co_await Latent::Seconds(FocusDuration);

    // プレイヤー位置に戻る
    co_await ReturnToPlayer(MoveSpeed);

    bIsInEventMode = false;
}

UE5Coro::TCoroutine<> UCameraHandlerComponent::FocusOnMultipleLocations(
    TArray<FVector> Locations,
    float DurationPerLocation,
    float MoveSpeed)
{
    using namespace UE5Coro;

    if (!Camera || Locations.Num() == 0)
        co_return;

    bIsInEventMode = true;

    for (const FVector& Location : Locations)
    {
        const float StopThreshold = 50.0f;
        const float MaxDuration = 10.0f;
        float ElapsedTime = 0.0f;

        // 各地点まで移動
        while (FVector::Dist(Camera->GetComponentLocation(), Location) > StopThreshold)
        {
            ElapsedTime += GetWorld()->GetDeltaSeconds();
            if (ElapsedTime > MaxDuration)
                break;

            FVector CurrentLocation = Camera->GetComponentLocation();
            FVector NewLocation = FMath::VInterpTo(CurrentLocation, Location,
                GetWorld()->GetDeltaSeconds(), MoveSpeed);
            Camera->SetWorldLocation(NewLocation);

            co_await Latent::NextTick();
        }

        Camera->SetWorldLocation(Location);

        // その地点で待機
        co_await Latent::Seconds(DurationPerLocation);
    }

    // プレイヤー位置に戻る
    co_await ReturnToPlayer(MoveSpeed);

    bIsInEventMode = false;
}

UE5Coro::TCoroutine<> UCameraHandlerComponent::ReturnToPlayer(float MoveSpeed)
{
    using namespace UE5Coro;

    if (!Camera || !GetOwner())
        co_return;

    FVector ReturnTarget = PreEventCameraLocation;
    FRotator ReturnRotation = PreEventCameraRotation;
    float ReturnFOV = PreEventFOV;

    const float StopThreshold = 10.0f;
    const float MaxDuration = 5.0f;
    float ElapsedTime = 0.0f;

    // 位置を戻す
    while (FVector::Dist(Camera->GetComponentLocation(), ReturnTarget) > StopThreshold)
    {
        ElapsedTime += GetWorld()->GetDeltaSeconds();
        if (ElapsedTime > MaxDuration)
            break;

        FVector CurrentLocation = Camera->GetComponentLocation();
        FVector NewLocation = FMath::VInterpTo(CurrentLocation, ReturnTarget,
            GetWorld()->GetDeltaSeconds(), MoveSpeed);
        Camera->SetWorldLocation(NewLocation);

        co_await Latent::NextTick();
    }

    Camera->SetWorldLocation(ReturnTarget);
    Camera->SetWorldRotation(ReturnRotation);
    Camera->SetFieldOfView(ReturnFOV);
}

UE5Coro::TCoroutine<> UCameraHandlerComponent::ZoomCamera(float TargetFOVValue, float Duration)
{
    using namespace UE5Coro;

    if (!Camera)
        co_return;

    float StartFOV = Camera->FieldOfView;
    float ElapsedTime = 0.0f;

    // ズームイン
    while (ElapsedTime < Duration)
    {
        ElapsedTime += GetWorld()->GetDeltaSeconds();
        float Alpha = FMath::Clamp(ElapsedTime / Duration, 0.0f, 1.0f);
        float NewFOV = FMath::Lerp(StartFOV, TargetFOVValue, Alpha);
        Camera->SetFieldOfView(NewFOV);

        co_await Latent::NextTick();
    }

    Camera->SetFieldOfView(TargetFOVValue);

    co_await Latent::Seconds(0.5f);

    // ズームアウト
    ElapsedTime = 0.0f;
    while (ElapsedTime < Duration)
    {
        ElapsedTime += GetWorld()->GetDeltaSeconds();
        float Alpha = FMath::Clamp(ElapsedTime / Duration, 0.0f, 1.0f);
        float NewFOV = FMath::Lerp(TargetFOVValue, OriginalFOV, Alpha);
        Camera->SetFieldOfView(NewFOV);

        co_await Latent::NextTick();
    }

    Camera->SetFieldOfView(OriginalFOV);
}

UE5Coro::TCoroutine<> UCameraHandlerComponent::ShakeCamera(float Intensity, float Duration)
{
    using namespace UE5Coro;

    if (!Camera)
        co_return;

    FVector OriginalLocation = Camera->GetComponentLocation();
    float ElapsedTime = 0.0f;

    while (ElapsedTime < Duration)
    {
        ElapsedTime += GetWorld()->GetDeltaSeconds();

        FVector RandomOffset(
            FMath::FRandRange(-Intensity, Intensity),
            FMath::FRandRange(-Intensity, Intensity),
            FMath::FRandRange(-Intensity, Intensity)
        );

        float DecayFactor = 1.0f - (ElapsedTime / Duration);
        RandomOffset *= DecayFactor;

        Camera->SetWorldLocation(OriginalLocation + RandomOffset);

        co_await Latent::NextTick();
    }

    Camera->SetWorldLocation(OriginalLocation);
}