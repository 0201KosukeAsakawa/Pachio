// GrowingTreeComponent.cpp
#include "Components/Color/GrowingTreeComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimSequence.h"
#include "Animation/AnimSingleNodeInstance.h"
#include "ColorUtilityLibrary.h"

// =======================
// 定数
// =======================
namespace
{
    /** 色判定のデフォルト許容誤差 */
    static constexpr float DEFAULT_COLOR_TOLERANCE = 0.1f;
}

// =======================
// コンストラクタ
// =======================
UGrowingTreeComponent::UGrowingTreeComponent()
    : GrowthMode(EGrowthMode::MeshSwap)
    , AnimationPlayRate(1.0f)
    , bEnableScaleChange(true)
    , StartScaleMultiplier(0.5f)
    , EndScaleMultiplier(2.0f)
    , GrowthDuration(1.5f)
    , WaterReadyDuration(5.0f)
    , WaterReadyColor(FLinearColor::Green)
    , GrowthTriggerColor(FLinearColor::Blue)
    , ColorTolerance(DEFAULT_COLOR_TOLERANCE)
    , CurrentStage(ETreeGrowthStage::Sapling)
    , bIsGrowing(false)
    , GrowthTimer(0.0f)
    , WaterReadyTimer(0.0f)
    , InitialScale(FVector::OneVector)
    , TargetScale(FVector::OneVector)
    , OriginalScale(FVector::OneVector)
{
    PrimaryComponentTick.bCanEverTick = true;
}

// =======================
// 初期化
// =======================
void UGrowingTreeComponent::BeginPlay()
{
    Super::BeginPlay();

    // アニメーションモードの場合、元のスケールを記録
    if (GrowthMode == EGrowthMode::Animation)
    {
        USkeletalMeshComponent* SkelMesh = GetSkeletalMeshComponent();
        if (SkelMesh)
        {
            OriginalScale = SkelMesh->GetRelativeScale3D();
        }
    }
}

void UGrowingTreeComponent::Initialize()
{
    Super::Initialize();

    // 初期段階は苗
    CurrentStage = ETreeGrowthStage::Sapling;

    // メッシュ切り替えモードの場合は初期メッシュを設定
    if (GrowthMode == EGrowthMode::MeshSwap)
    {
        SetMeshForStage(CurrentStage);
    }

    UE_LOG(LogTemp, Log, TEXT("[GrowingTree] Initialized (Mode: %s): %s"),
        GrowthMode == EGrowthMode::MeshSwap ? TEXT("MeshSwap") : TEXT("Animation"),
        *GetOwner()->GetName());
}

void UGrowingTreeComponent::TickComponent(float DeltaTime, ELevelTick TickType,
    FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    // 成長アニメーション更新
    if (bIsGrowing)
    {
        UpdateGrowth(DeltaTime);
    }
}

// =======================
// 色受信処理
// =======================
void UGrowingTreeComponent::ActivateDirect(const FLinearColor& InColor)
{
    Super::ActivateDirect(InColor);

    // 成長済みの場合は何もしない
    if (CurrentStage == ETreeGrowthStage::FullyGrown)
    {
        return;
    }

    // 成長中の場合は処理しない
    if (bIsGrowing)
    {
        return;
    }

    // 緑色を受けた場合 → 水分補給状態へ
    if (UColorUtilityLibrary::IsHueSimilar(InColor, WaterReadyColor))
    {
        ActivateWaterReady();
    }
    // 青色を受けた場合 → 成長開始（水分補給状態の時のみ）
    else if (UColorUtilityLibrary::IsHueSimilar(InColor, GrowthTriggerColor))
    {
        if (CurrentStage == ETreeGrowthStage::WaitingForWater)
        {
            StartGrowth();
        }
    }
}

// =======================
// 水分補給
// =======================
void UGrowingTreeComponent::ActivateWaterReady()
{
    if (CurrentStage != ETreeGrowthStage::Sapling)
    {
        return;
    }

    SetGrowthStage(ETreeGrowthStage::WaitingForWater);
    WaterReadyTimer = 0.0f;

    // 水分エフェクト再生
    PlayWaterEffect();

    // 色を緑に変更
    ApplyColorToMaterial(WaterReadyColor);

    UE_LOG(LogTemp, Log, TEXT("[GrowingTree] Water ready: %s"), *GetOwner()->GetName());
}

// =======================
// 成長処理（苗→木）
// =======================
void UGrowingTreeComponent::StartGrowth()
{
    if (bIsGrowing)
    {
        return;
    }

    bIsGrowing = true;
    GrowthTimer = 0.0f;

    // 成長段階を進める
    SetGrowthStage(ETreeGrowthStage::Growing);

    // モードに応じた処理
    if (GrowthMode == EGrowthMode::MeshSwap)
    {
        // メッシュ切り替えモード：成木メッシュに変更してスケールアニメーション
        SetMeshForStage(ETreeGrowthStage::FullyGrown);

        UStaticMeshComponent* MeshComp = GetTreeMeshComponent();
        if (MeshComp)
        {
            InitialScale = FVector(0.1f, 0.1f, 0.1f);
            TargetScale = FVector(1.0f, 1.0f, 1.0f);
            MeshComp->SetRelativeScale3D(InitialScale);
        }
    }
    else if (GrowthMode == EGrowthMode::Animation)
    {
        // アニメーションモード：アニメーション再生とスケール変更
        PlayGrowthAnimation();

        if (bEnableScaleChange)
        {
            USkeletalMeshComponent* SkelMesh = GetSkeletalMeshComponent();
            if (SkelMesh)
            {
                InitialScale = OriginalScale * StartScaleMultiplier;
                TargetScale = OriginalScale * EndScaleMultiplier;
                SkelMesh->SetRelativeScale3D(InitialScale);
            }
        }
    }

    // エフェクトとサウンド再生
    PlayGrowthEffect();

    if (GrowthSound)
    {
        UGameplayStatics::PlaySoundAtLocation(
            this,
            GrowthSound,
            GetOwner()->GetActorLocation()
        );
    }

    UE_LOG(LogTemp, Log, TEXT("[GrowingTree] Growth started (Sapling -> Tree): %s"),
        *GetOwner()->GetName());
}

void UGrowingTreeComponent::UpdateGrowth(float DeltaTime)
{
    GrowthTimer += DeltaTime;
    float GrowthProgress = FMath::Clamp(GrowthTimer / GrowthDuration, 0.0f, 1.0f);

    // カーブを使用する場合
    if (GrowthCurve)
    {
        GrowthProgress = GrowthCurve->GetFloatValue(GrowthProgress);
    }
    else
    {
        // デフォルトでイーズアウト（スムーズな減速）
        GrowthProgress = FMath::InterpEaseOut(0.0f, 1.0f, GrowthProgress, 2.0f);
    }

    // モードに応じてスケール更新
    if (GrowthMode == EGrowthMode::MeshSwap)
    {
        UStaticMeshComponent* MeshComp = GetTreeMeshComponent();
        if (MeshComp)
        {
            FVector CurrentScale = FMath::Lerp(InitialScale, TargetScale, GrowthProgress);
            MeshComp->SetRelativeScale3D(CurrentScale);
        }
    }
    else if (GrowthMode == EGrowthMode::Animation && bEnableScaleChange)
    {
        UpdateScaleAnimation(DeltaTime);
    }

    // 成長完了チェック
    if (GrowthTimer >= GrowthDuration)
    {
        CompleteGrowth();
    }
}

void UGrowingTreeComponent::CompleteGrowth()
{
    bIsGrowing = false;
    GrowthTimer = 0.0f;

    // 最終スケールを確定
    if (GrowthMode == EGrowthMode::MeshSwap)
    {
        UStaticMeshComponent* MeshComp = GetTreeMeshComponent();
        if (MeshComp)
        {
            MeshComp->SetRelativeScale3D(TargetScale);
        }
    }
    else if (GrowthMode == EGrowthMode::Animation && bEnableScaleChange)
    {
        USkeletalMeshComponent* SkelMesh = GetSkeletalMeshComponent();
        if (SkelMesh)
        {
            SkelMesh->SetRelativeScale3D(TargetScale);
        }
    }

    // 完全成長状態へ
    SetGrowthStage(ETreeGrowthStage::FullyGrown);

    UE_LOG(LogTemp, Log, TEXT("[GrowingTree] Growth completed: %s"), *GetOwner()->GetName());
}

// =======================
// メッシュ切り替えモード用
// =======================
void UGrowingTreeComponent::SetMeshForStage(ETreeGrowthStage Stage)
{
    if (GrowthMode != EGrowthMode::MeshSwap)
    {
        return;
    }

    UStaticMeshComponent* MeshComp = GetTreeMeshComponent();
    if (!MeshComp)
    {
        return;
    }

    UStaticMesh* MeshToSet = nullptr;

    switch (Stage)
    {
    case ETreeGrowthStage::Sapling:
    case ETreeGrowthStage::WaitingForWater:
        MeshToSet = SaplingMesh;
        break;

    case ETreeGrowthStage::Growing:
    case ETreeGrowthStage::FullyGrown:
        MeshToSet = FullyGrownMesh;
        break;
    }

    if (MeshToSet)
    {
        MeshComp->SetStaticMesh(MeshToSet);

        // 通常の成長段階ではスケールを1に戻す
        if (Stage != ETreeGrowthStage::Growing)
        {
            MeshComp->SetRelativeScale3D(FVector::OneVector);
        }
    }
}

// =======================
// アニメーションモード用
// =======================
void UGrowingTreeComponent::PlayGrowthAnimation()
{
    if (GrowthMode != EGrowthMode::Animation || !GrowthAnimation)
    {
        return;
    }

    USkeletalMeshComponent* SkelMesh = GetSkeletalMeshComponent();
    if (!SkelMesh)
    {
        UE_LOG(LogTemp, Warning, TEXT("[GrowingTree] SkeletalMeshComponent not found for animation mode: %s"),
            *GetOwner()->GetName());
        return;
    }

    // アニメーションを再生
    SkelMesh->PlayAnimation(GrowthAnimation, false);
    SkelMesh->SetPlayRate(AnimationPlayRate);

    UE_LOG(LogTemp, Log, TEXT("[GrowingTree] Playing growth animation: %s"), *GetOwner()->GetName());
}

void UGrowingTreeComponent::UpdateScaleAnimation(float DeltaTime)
{
    USkeletalMeshComponent* SkelMesh = GetSkeletalMeshComponent();
    if (!SkelMesh)
    {
        return;
    }

    float GrowthProgress = FMath::Clamp(GrowthTimer / GrowthDuration, 0.0f, 1.0f);

    // カーブを使用する場合
    if (GrowthCurve)
    {
        GrowthProgress = GrowthCurve->GetFloatValue(GrowthProgress);
    }
    else
    {
        GrowthProgress = FMath::InterpEaseOut(0.0f, 1.0f, GrowthProgress, 2.0f);
    }

    FVector CurrentScale = FMath::Lerp(InitialScale, TargetScale, GrowthProgress);
    SkelMesh->SetRelativeScale3D(CurrentScale);
}

// =======================
// 成長段階管理
// =======================
void UGrowingTreeComponent::SetGrowthStage(ETreeGrowthStage NewStage)
{
    if (CurrentStage == NewStage)
    {
        return;
    }

    ETreeGrowthStage OldStage = CurrentStage;
    CurrentStage = NewStage;

    // メッシュ切り替えモードかつ成長中でない場合はメッシュを即座に変更
    if (GrowthMode == EGrowthMode::MeshSwap && !bIsGrowing)
    {
        SetMeshForStage(NewStage);
    }

    UE_LOG(LogTemp, Log, TEXT("[GrowingTree] Stage changed: %s -> %s (%s)"),
        *UEnum::GetValueAsString(OldStage),
        *UEnum::GetValueAsString(NewStage),
        *GetOwner()->GetName());
}

// =======================
// エフェクト
// =======================
void UGrowingTreeComponent::PlayGrowthEffect()
{
    if (GrowthEffect)
    {
        UNiagaraFunctionLibrary::SpawnSystemAtLocation(
            GetWorld(),
            GrowthEffect,
            GetOwner()->GetActorLocation()
        );
    }
}

void UGrowingTreeComponent::PlayWaterEffect()
{
    if (WaterEffect)
    {
        UNiagaraFunctionLibrary::SpawnSystemAtLocation(
            GetWorld(),
            WaterEffect,
            GetOwner()->GetActorLocation()
        );
    }

    if (WaterSound)
    {
        UGameplayStatics::PlaySoundAtLocation(
            this,
            WaterSound,
            GetOwner()->GetActorLocation()
        );
    }
}

// =======================
// ヘルパー関数
// =======================
UStaticMeshComponent* UGrowingTreeComponent::GetTreeMeshComponent() const
{
    return GetOwner()->GetComponentByClass<UStaticMeshComponent>();
}

USkeletalMeshComponent* UGrowingTreeComponent::GetSkeletalMeshComponent() const
{
    return GetOwner()->GetComponentByClass<USkeletalMeshComponent>();
}