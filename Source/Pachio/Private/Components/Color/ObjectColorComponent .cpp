#include "Components/Color/ObjectColorComponent.h"
#include "Components/Color/ColorReactiveComponent.h"
#include "Components/Beat/BeatScalerComponent.h"
#include "Interface/ColorFilterInterface.h"
#include "Manager/LevelManager.h"
#include "Manager/ColorManager.h"
#include "Sound/SoundManager.h"
#include "FunctionLibrary.h"


// ====================================================================
// UObjectColorComponent - 高レベルAPI層
// 初期化、マネージャー連携、イベント処理を担当
// ====================================================================


UObjectColorComponent::UObjectColorComponent()
    : CurrentColor(FLinearColor::White)
    , InitialColor(FLinearColor::White)
    , bApplyColorToMaterial(true)
    , bEnableColorAction(true)
    , bEnableBeatEffect(true)
    , bTreatAsColorVariable(false)
    , bUseComplementaryColor(false)
    , bColorMatched(false)
    , bSelected(false)
    , bColorChangeable(true)
    , bLocked(false)
{
    BeatScaler = CreateDefaultSubobject<UBeatScalerComponent>(TEXT("BeatScaler"));
    ColorReactive = CreateDefaultSubobject<UColorReactiveComponent>(TEXT("ColorReactiveComponent"));
}

// =======================
// 初期化フロー
// =======================

void UObjectColorComponent::Initialize()
{
    InitializeColorLogic();
    RegisterToColorManager();
    SetupMaterial();
    BindSoundEvents();
}

void UObjectColorComponent::InitializeColorLogic()
{
    if (!ReactiveComponentClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("ReactiveComponentClass is not set in %s"),
            *GetOwner()->GetName());
        return;
    }
    ColorReactive = NewObject<UColorReactiveComponent>(this, ReactiveComponentClass);
    InitialColor = ALevelManager::GetInstance(GetWorld())->GetColorManager()->GetEffectColor(EffectType);

    CurrentColor = InitialColor;

    // ColorReactiveコンポーネントを生成・登録
    if (!ColorReactive)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to create ColorReactive component in %s"),
            *GetOwner()->GetName());
        return;
    }

    //ColorReactive->RegisterComponent();

    //ColorReactive->Activate(true);

    // 初期化（色、エフェクト、Niagara設定）
    ColorReactive->Initialize(InitialColor,true,GetOwner());
    ColorReactive->SetEffectType(EffectType);
    ColorReactive->SetupNiagaraActors(NiagaraActors);
}

void UObjectColorComponent::RegisterToColorManager()
{
    UColorManager* ColorManager = GetColorManager();
    if (!ColorManager)
    {
        UE_LOG(LogTemp, Warning, TEXT("ColorManager not found for %s"),
            *GetOwner()->GetName());
        return;
    }

    ColorManager->RegisterTarget(TargetType, GetOwner());
}

void UObjectColorComponent::SetupMaterial()
{
    if (!bApplyColorToMaterial)
    {
        return;
    }

    // カラーマネージャから初期色を取得
    const UColorManager* ColorManager = GetColorManager();
    if (ColorManager)
    {
        InitialColor = ColorManager->GetEffectColor(EffectType);
    }

    USkeletalMeshComponent* Mesh = GetMeshComponent();
    if (!Mesh)
    {
        return;
    }

    // カスタムデプスレンダリング設定
    Mesh->SetRenderCustomDepth(true);
    Mesh->SetCustomDepthStencilValue(CUSTOM_DEPTH_STENCIL_VALUE);

    // ダイナミックマテリアル作成と色適用
    UMaterialInstanceDynamic* DynMaterial = Mesh->CreateAndSetMaterialInstanceDynamic(MATERIAL_SLOT_INDEX);
    if (DynMaterial)
    {
        DynMaterial->SetVectorParameterValue(FName("BaseColor"), InitialColor);
    }
}

void UObjectColorComponent::BindSoundEvents()
{
    const ALevelManager* LevelManager = GetLevelManager();
    if (!LevelManager)
    {
        return;
    }

    USoundManager* SoundManager = Cast<USoundManager>(
        LevelManager->GetSoundManager().GetObject()
    );

    if (!SoundManager)
    {
        UE_LOG(LogTemp, Warning, TEXT("SoundManager not found for %s"),
            *GetOwner()->GetName());
        return;
    }

    SoundManager->OnBeatDetected.AddDynamic(this, &UObjectColorComponent::PlayBeatAnimation);
}

// =======================
// イベント処理
// =======================

void UObjectColorComponent::PlayBeatAnimation()
{
    if (!bEnableBeatEffect || !BeatScaler)
    {
        return;
    }

    BeatScaler->PlayBeat();
}

// =======================
// 色操作API
// =======================

void UObjectColorComponent::SetColor(const FLinearColor& NewColor,
    const FEffectMatchResult& MatchResult)
{
    CurrentColor = NewColor;

    if (ColorReactive == nullptr)
    {
        return;
    }

    // マテリアルへ反映
    if (bApplyColorToMaterial)
    {
        ColorReactive->ApplyColorToMaterial(CurrentColor);
    }

    // エフェクトタイプ更新
    ColorReactive->SetEffectType(MatchResult.ClosestEffect);


    // カラーマッチング処理
    ProcessColorMatching(NewColor, MatchResult);
}

void UObjectColorComponent::ResetColor(const FEffectMatchResult& MatchResult)
{
    SetColor(InitialColor, MatchResult);
}

void UObjectColorComponent::SetCurrentColorOnly(const FLinearColor& NewColor)
{
    CurrentColor = NewColor;
}

void UObjectColorComponent::ProcessColorMatching(const FLinearColor& NewColor,
    const FEffectMatchResult& MatchResult)
{
    if (!bEnableColorAction || !ColorReactive)
    {
        return;
    }

    const UColorManager* ColorManager = GetColorManager();
    if (!ColorManager)
    {
        return;
    }

    FLinearColor WorldColor = ColorManager->GetWorldColor();

    // 色変数として扱う場合はマテリアルに反映
    if (bTreatAsColorVariable)
    {
        ColorReactive->ApplyColorToMaterial(WorldColor);
    }

    // 色一致判定を更新
    bColorMatched = GetHueAngleDistance(InitialColor, WorldColor);

    if (!bColorMatched)
    {
        ColorReactive->OnColorMismatched(WorldColor);
    }
    else
    {
        ColorReactive->OnColorMatched(WorldColor);
    }
}

void UObjectColorComponent::ApplyColorToMaterial(const FLinearColor& Color)
{
    if (ColorReactive)
    {
        ColorReactive->ApplyColorToMaterial(Color);
    }
}

// =======================
// 状態の取得と設定
// =======================

void UObjectColorComponent::SetColorMatched(bool bMatched)
{
    bColorMatched = bMatched;
}

void UObjectColorComponent::SetSelected(bool bInSelected)
{
    bSelected = bInSelected;

    if (ColorReactive)
    {
        ColorReactive->SetSelectMode(bSelected);
    }
}

void UObjectColorComponent::SetLocked(bool bInLocked)
{
    bLocked = bInLocked;
}

bool UObjectColorComponent::IsHidden() const
{
    return ColorReactive && ColorReactive->IsHidden();
}

// =======================
// 色判定API（ColorReactiveに委譲）
// =======================

bool UObjectColorComponent::HasColorChanged() const
{
    return ColorReactive &&
        ColorReactive->HasColorChanged(CurrentColor, InitialColor);
}

bool UObjectColorComponent::HasColorChanged(const FLinearColor& CompareColor) const
{
    return ColorReactive &&
        ColorReactive->HasColorChanged(CurrentColor, CompareColor);
}

bool UObjectColorComponent::MatchesColorByRGB(const FEffectMatchResult& MatchResult,
    const FLinearColor& FilterColor,
    bool bUseComplementary) const
{
    if (!ColorReactive)
    {
        return false;
    }

    FLinearColor CompareColor = bUseComplementary ?
        ColorReactive->GetComplementaryColor(FilterColor) : FilterColor;

    return ColorReactive->IsRGBDistanceWithinThreshold(
        CurrentColor,
        CompareColor,
        0.3
    );
}

bool UObjectColorComponent::IsSimilarColor(const FLinearColor& ColorA,
    const FLinearColor& ColorB,
    float Tolerance) const
{
    return ColorReactive &&
        ColorReactive->IsColorDegreeDistanceWithinThreshold(ColorA, ColorB, Tolerance);
}

bool UObjectColorComponent::IsSimilarColor(const FLinearColor& FilterColor,
    float Tolerance) const
{
    return ColorReactive &&
        ColorReactive->IsColorDegreeDistanceWithinThreshold(CurrentColor, FilterColor, Tolerance);
}

bool UObjectColorComponent::GetHueAngleDistance(const FLinearColor& ColorA, const FLinearColor& ColorB, float Tolerance)
{
    if (!ColorReactive)
        return false;

   float deg = ColorReactive->GetHueAngleDistance(ColorA, ColorB);

   return deg >= Tolerance;
}

// =======================
// ヘルパー関数
// =======================

USkeletalMeshComponent* UObjectColorComponent::GetMeshComponent() const
{
    return UFunctionLibrary::FindComponentByName<USkeletalMeshComponent>(GetOwner(), TEXT("Mesh"));
}

ALevelManager* UObjectColorComponent::GetLevelManager() const
{
    return ALevelManager::GetInstance(GetWorld());
}

UColorManager* UObjectColorComponent::GetColorManager() const
{
    const ALevelManager* LevelManager = GetLevelManager();
    return LevelManager ? LevelManager->GetColorManager() : nullptr;
}