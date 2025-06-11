#include "Manager/ColorManager.h"
#include "Kismet/GameplayStatics.h"
#include "Components/PostProcessComponent.h"
#include "Engine/PostProcessVolume.h"
#include "Interface/ColorFilterInterface.h"
#include "Components/ColorControllerComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Logic/ColorManager/EffectColorMatcher.h"
#include "Logic/ColorManager/ColorTargetRegistry.h"

// 色とバフ効果の対応を管理するクラス
void UColorManager::Init()
{
    // プレイヤーのコントローラーから色変更イベントを受け取る
    BindController();
    // ポストプロセスボリュームとマテリアル初期化（視覚効果用）
    InitializePostEffect();
    ColorTargetRegistry = NewObject<UColorTargetRegistry>();
    EffectColorMatcher = NewObject<UEffectColorMatcher>();
}

// 色を反映し、ターゲットに通知する
void UColorManager::ApplyColor(FLinearColor NewColor, EColorTargetType Mode)
{
    if (!ColorTargetRegistry)
        return;


    switch (Mode)
    {
    case EColorTargetType::Layer:
        if (PostProcessMID)
        {
            // ポストプロセスマテリアルに色を適用
            PostProcessMID->SetVectorParameterValue(TEXT("FilterColor"), NewColor);
        }
        // 常時反応するターゲット（例：UIなど）に通知
        ColorTargetRegistry-> NotifyTargets(EColorTargetType::Responders, NewColor);
        break;

    case EColorTargetType::Object:
    case EColorTargetType::Background:
        // 指定されたモードのターゲットに通知
        ColorTargetRegistry-> NotifyTargets(Mode, NewColor);
        break;

    default:
        break;
    }
}

// 色変化に反応するターゲットを登録
void UColorManager::RegisterTarget(EColorTargetType Mode, TScriptInterface<IColorReactiveInterface> Target)
{
    if (!ColorTargetRegistry)
        return;

    ColorTargetRegistry->RegisterTarget(Mode, Target);
}

FEffectMatchResult UColorManager::GetClosestEffectByHue(const FLinearColor& InputColor)
{
    if (!EffectColorMatcher)
        return FEffectMatchResult();

    return EffectColorMatcher->GetClosestEffectByHue(InputColor);
}

// プレイヤーの色コントローラーとイベント接続
void UColorManager::BindController()
{
    APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    if (PlayerPawn)
    {
        UColorControllerComponent* ColorController = PlayerPawn->FindComponentByClass<UColorControllerComponent>();
        if (ColorController && !ColorController->OnColorChanged.IsAlreadyBound(this, &UColorManager::ApplyColor))
        {
            // 色変更イベントにバインド
            ColorController->OnColorChanged.AddDynamic(this, &UColorManager::ApplyColor);
        }
    }
}

// ポストプロセス用マテリアルの初期化（ビジュアルフィルター表示などに使用）
void UColorManager::InitializePostEffect()
{
    TArray<AActor*> FoundVolumes;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), APostProcessVolume::StaticClass(), FoundVolumes);

    if (FoundVolumes.Num() < 0)
        return;

    APostProcessVolume* PostProcessVolume = Cast<APostProcessVolume>(FoundVolumes[0]);

    if (PostProcessVolume && PostProcessMaterial)
    {
        // マテリアルインスタンスを作成しポストプロセスに適用
        PostProcessMID = UMaterialInstanceDynamic::Create(PostProcessMaterial, this);
        PostProcessVolume->Settings.WeightedBlendables.Array.Add(FWeightedBlendable(1.0f, PostProcessMID));
    }
}
