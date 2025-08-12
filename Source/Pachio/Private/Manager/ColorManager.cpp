#include "Manager/ColorManager.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Manager/LevelManager.h"
#include "UI/UIManager.h"
#include "UI/ColorLens.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/PostProcessVolume.h"
#include "Interface/ColorFilterInterface.h"
#include "Components/Color/ColorControllerComponent.h"
#include "Components/PostProcessComponent.h"
#include "Logic/ColorManager/EffectColorMatcher.h"
#include "Logic/ColorManager/ColorTargetRegistry.h"

// 色とバフ効果の対応を管理するクラス
void UColorManager::Init()
{    
    // ColorTargetRegistryClass が設定されている場合にインスタンス化
    if (ColorTargetRegistryClass)
    {
        // クラスを基にインスタンスを生成
        ColorTargetRegistry = NewObject<UColorTargetRegistry>(this, ColorTargetRegistryClass);

    }

    EffectColorMatcher = NewObject<UEffectColorMatcher>();
    // プレイヤーのコントローラーから色変更イベントを受け取る
    BindController();
    // ポストプロセスボリュームとマテリアル初期化（視覚効果用）
    InitializePostEffect();

}

// 色を反映し、ターゲットに通知する
void UColorManager::ApplyColor(FLinearColor NewColor, EColorTargetType Mode)
{
    if (!ColorTargetRegistry)
        return;
    FEffectMatchResult result = GetClosestEffectByHue(NewColor);
    ColorTargetRegistry->ApplyColor(NewColor, Mode, result);
}

void UColorManager::ColorEvent(FName EventID, FLinearColor NewColor)
{
    if (!ColorTargetRegistry)
        return;
    FEffectMatchResult result = GetClosestEffectByHue(NewColor);
    ColorTargetRegistry->ColorEvent(EventID, NewColor, result);
}

void UColorManager::SetColorTarget(IColorReactiveInterface* target)
{
    ColorTargetRegistry->SetColorTarget(target);
}

void UColorManager::ResetColorTarget()
{
    ColorTargetRegistry->ResetColorTarget();
}

// 色変化に反応するターゲットを登録
void UColorManager::RegisterTarget(EColorTargetType Mode, TScriptInterface<IColorReactiveInterface> Target)
{
    if (!this || !ColorTargetRegistry)
        return;
    ColorTargetRegistry->RegisterTarget(Mode, Target);
}

float UColorManager::GetColorDistanceRGB(const FLinearColor& A)
{
    return EffectColorMatcher->GetColorDistanceRGB(A, ColorTargetRegistry->GetPostProcessColor());
}

float UColorManager::GetColorDistanceRGB(const FLinearColor& A, const FLinearColor& B)
{
    return EffectColorMatcher->GetColorDistanceRGB(A,B);
}

FEffectMatchResult UColorManager::GetClosestEffectByHue()
{
    if (!EffectColorMatcher)
        return FEffectMatchResult();

    return EffectColorMatcher->GetClosestEffectByHue(ColorTargetRegistry->GetPostProcessColor());
}

FEffectMatchResult UColorManager::GetClosestEffectByHue(const FLinearColor& InputColor)
{
    if (!EffectColorMatcher)
        return FEffectMatchResult();

    return EffectColorMatcher->GetClosestEffectByHue(InputColor);
}

FLinearColor UColorManager::GetWorldColor() const
{
    return ColorTargetRegistry->GetPostProcessColor();
}

// プレイヤーの色コントローラーとイベント接続
void UColorManager::BindController()
{
    APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    if (PlayerPawn == nullptr)
        return;

    UColorControllerComponent* ColorController = PlayerPawn->FindComponentByClass<UColorControllerComponent>();
    if (ColorController == nullptr)
        return;

    if (!ColorController->OnColorChanged.IsAlreadyBound(this, &UColorManager::ApplyColor))
    {
        // 色変更イベントにバインド
        ColorController->OnColorChanged.AddDynamic(this, &UColorManager::ApplyColor);
    }

    ALevelManager* levelManager = ALevelManager::GetInstance(GetWorld());
    if (levelManager == nullptr)
        return;
    if (levelManager->GetUIManager() == nullptr)
        return;
    if (levelManager->GetUIManager()->GetColorLens() == nullptr)
        return;
    ColorController->AnimationDelegate.BindUObject(levelManager->GetUIManager()->GetColorLens(), &UColorLens::Animation);
}

// ポストプロセス用マテリアルの初期化（ビジュアルフィルター表示などに使用）
void UColorManager::InitializePostEffect()
{
    if (!ColorTargetRegistry)
        return;
    ColorTargetRegistry->InitializePostEffect();
}
