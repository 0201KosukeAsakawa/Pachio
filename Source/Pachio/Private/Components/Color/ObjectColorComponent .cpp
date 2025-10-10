#include "Components/Color/ObjectColorComponent.h"
#include "Components/Color/ColorReactiveComponent.h"
#include "Components/Beat/BeatScalerComponent.h"
#include "Interface/ColorFilterInterface.h"
#include "Manager/LevelManager.h"
#include "Manager/ColorManager.h"
#include "Sound/SoundManager.h"
#include "FunctionLibrary.h"


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
{
    // ビート演出用コンポーネントを生成
    BeatScaler = CreateDefaultSubobject<UBeatScalerComponent>(TEXT("BeatScaler"));
}

void UObjectColorComponent::Initialize()
{
    // 各種初期化処理を順次実行
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

    // 現在色を初期色に設定
    CurrentColor = InitialColor;

    // 色リアクティブコンポーネントを生成
    ColorReactive = NewObject<UColorReactiveComponent>(this, ReactiveComponentClass);
    if (!ColorReactive)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to create ColorReactive component in %s"),
            *GetOwner()->GetName());
        return;
    }

    // 登録 & 有効化
    ColorReactive->RegisterComponent();
    ColorReactive->Activate(true);

    // 色エフェクトとNiagaraを初期化
    ColorReactive->InitColorEffectAndNiagara(InitialColor, EffectType, NiagaraActors);
    ColorReactive->Initialize(bTreatAsColorVariable);
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

    // カラーマネージャに自身を登録
    ColorManager->RegisterTarget(TargetType, GetOwner());
}

void UObjectColorComponent::SetupMaterial()
{
    if (!bApplyColorToMaterial)
    {
        return;
    }

    // 初期色をカラーマネージャから取得
    const UColorManager* ColorManager = GetColorManager();
    if (ColorManager)
    {
        InitialColor = ColorManager->GetEffectColor(EffectType);
    }

    // メッシュを取得して色を適用
    USkeletalMeshComponent* Mesh = GetMeshComponent();
    if (!Mesh)
    {
        return;
    }

    // カスタムデプスレンダリングを有効化
    Mesh->SetRenderCustomDepth(true);
    Mesh->SetCustomDepthStencilValue(CUSTOM_DEPTH_STENCIL_VALUE);

    // ダイナミックマテリアルを作成して色を設定
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

    // サウンドマネージャを取得
    USoundManager* SoundManager = Cast<USoundManager>(
        LevelManager->GetSoundManager().GetObject()
    );

    if (!SoundManager)
    {
        UE_LOG(LogTemp, Warning, TEXT("SoundManager not found for %s"),
            *GetOwner()->GetName());
        return;
    }

    // ビートイベントに登録
    SoundManager->OnBeatDetected.AddDynamic(this, &UObjectColorComponent::PlayBeatAnimation);
}

void UObjectColorComponent::PlayBeatAnimation()
{
    // ビート演出が無効なら何もしない
    if (!bEnableBeatEffect || !BeatScaler)
    {
        return;
    }

    // ビート演出を実行
    BeatScaler->PlayBeat();
}

void UObjectColorComponent::ProcessColorMatching(const FLinearColor& NewColor,
    const FEffectMatchResult& MatchResult)
{
    if (!bEnableColorAction || !ColorReactive)
    {
        return;
    }

    // 色変数扱いならマテリアルに反映
    if (bTreatAsColorVariable)
    {
        ApplyColorToMaterial(NewColor);
    }

    // 色一致判定を更新
    bColorMatched = ColorReactive->IsRGBDistancewithinThreshold(
        MatchResult,
        NewColor,
        bUseComplementaryColor
    );
}

void UObjectColorComponent::SetColor(const FLinearColor& NewColor,
    const FEffectMatchResult& MatchResult)
{
    // 現在色を更新
    CurrentColor = NewColor;

    // マテリアルへ反映
    if (bApplyColorToMaterial)
    {
        ApplyColorToMaterial(CurrentColor);
    }

    // リアクティブコンポーネントへ色適用
    if (ColorReactive)
    {
        ColorReactive->InitColorEffectAndNiagara(
            CurrentColor,
            MatchResult.ClosestEffect,
            NiagaraActors
        );
    }

    // カラーマネージャが存在するならマッチング処理実行
    const UColorManager* ColorManager = GetColorManager();
    if (ColorManager)
    {
        ProcessColorMatching(ColorManager->GetWorldColor(), MatchResult);
    }
}

void UObjectColorComponent::ResetColor(const FEffectMatchResult& MatchResult)
{
    // 初期色にリセット
    SetColor(InitialColor, MatchResult);
}

void UObjectColorComponent::SetCurrentColorOnly(const FLinearColor& NewColor)
{
    // 内部的に色だけ更新（マテリアルやエフェクトには反映しない）
    CurrentColor = NewColor;
}

void UObjectColorComponent::SetColorMatched(bool bMatched)
{
    // 色一致フラグを更新
    bColorMatched = bMatched;
}

void UObjectColorComponent::SetSelected(bool bInSelected)
{
    // 選択状態を更新
    bSelected = bInSelected;

    // リアクティブ側にも伝える
    if (ColorReactive)
    {
        ColorReactive->SetSelectMode(bSelected);
    }
}

bool UObjectColorComponent::HasColorChanged() const
{
    // 初期色と現在の色が異なるかを確認
    return ColorReactive &&
        !ColorReactive->IsColorDegreeDistanceWithinThreshold(InitialColor);
}

bool UObjectColorComponent::HasColorChanged(const FLinearColor& CompareColor) const
{
    // 指定色と現在の色が異なるかを確認
    return ColorReactive &&
        !ColorReactive->IsColorDegreeDistanceWithinThreshold(CompareColor);
}

bool UObjectColorComponent::MatchesColorByRGB(const FEffectMatchResult& MatchResult,
    const FLinearColor& FilterColor,
    bool bUseComplementary) const
{
    // マッチ判定を実行
    return ColorReactive &&
        ColorReactive->IsRGBDistancewithinThreshold(
            MatchResult,
            FilterColor,
            bUseComplementary
        );
}

bool UObjectColorComponent::IsSimilarColor(const FLinearColor& ColorA,
    const FLinearColor& ColorB,
    float Tolerance) const
{
    // 2色間の一致確認
    return ColorReactive &&
        ColorReactive->IsColorDegreeDistanceWithinThreshold(ColorA, ColorB, Tolerance);
}

bool UObjectColorComponent::IsSimilarColor(const FLinearColor& FilterColor,
    float Tolerance) const
{
    // フィルター色と現在の色の一致確認
    return ColorReactive &&
        ColorReactive->IsColorDegreeDistanceWithinThreshold(FilterColor, Tolerance);
}

bool UObjectColorComponent::IsHidden() const
{
    // 非表示判定
    return ColorReactive && ColorReactive->IsHidden();
}

void UObjectColorComponent::ApplyColorToMaterial(const FLinearColor& Color)
{
    // マテリアルに色を適用
    if (ColorReactive)
    {
        ColorReactive->ApplyColorToMaterial(Color);
    }
}

// =======================
// ヘルパー関数
// =======================

USkeletalMeshComponent* UObjectColorComponent::GetMeshComponent() const
{
    // "Mesh" という名前の SkeletalMeshComponent を探す
    return UFunctionLibrary::FindComponentByName<USkeletalMeshComponent>(GetOwner(),TEXT("Mesh"));
}

ALevelManager* UObjectColorComponent::GetLevelManager() const
{
    // レベルマネージャ取得
    return ALevelManager::GetInstance(GetWorld());
}

UColorManager* UObjectColorComponent::GetColorManager() const
{
    // カラーマネージャ取得
    const ALevelManager* LevelManager = GetLevelManager();
    return LevelManager ? LevelManager->GetColorManager() : nullptr;
}