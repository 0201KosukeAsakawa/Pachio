// Fill out your copyright notice in the Description page of Project Settings.


#include "Logic/ColorManager/ColorTargetRegistry.h"
#include "Logic/ColorManager/EffectColorMatcher.h"
#include "Interface/ColorFilterInterface.h"
#include "Kismet/GameplayStatics.h"


// =======================
// 色の適用処理
// =======================

void UColorTargetRegistry::ApplyColor(FLinearColor NewColor, EColorTargetType Mode, FEffectMatchResult effect)
{
    switch (Mode)
    {
    case EColorTargetType::WorldColor:
        if (PostProcessMID)
        {
            // ポストプロセスマテリアルに色を適用（画面全体のカラー演出）
            PostProcessMID->SetVectorParameterValue(TEXT("FilterColor"), NewColor);
        }
        // 指定されたモードのターゲットに通知
        NotifyTargets(Mode, NewColor, effect);
        // 常時反応するターゲット（例：UIなど Responders）にも通知
        NotifyTargets(EColorTargetType::Responders, NewColor, effect);
        break;

    case EColorTargetType::ObjectColor:
        // 特定オブジェクトに色を適用
        if (!TargetObject)
            return;
        TargetObject->ColorAction(NewColor, effect);
        break;

    default:
        break;
    }

    // 色適用イベントをブロードキャスト
    OnColorApplied.Broadcast(Mode, NewColor);
}

// =======================
// イベント用の色適用処理
// =======================

void UColorTargetRegistry::ColorEvent(FName EventID, FLinearColor NewColor, FEffectMatchResult effect)
{
    // Event ターゲットが存在しなければ終了
    if (!ColorResponseTargets.Contains(EColorTargetType::Event))
    {
        return;
    }

    auto& Instances = ColorResponseTargets[EColorTargetType::Event].Instances;
    if (Instances.Num() == 0)
        return;

    // EventID が一致するターゲットにのみ通知
    for (auto& TargetInstance : Instances)
    {
        if (TargetInstance->GetColorEventID() != EventID)
            continue;

        TargetInstance->ColorAction(NewColor, effect);
    }
}

// =======================
// ターゲットの選択・リセット
// =======================

// 特定のオブジェクトを現在の色ターゲットとして設定
void UColorTargetRegistry::SetColorTarget(IColorReactiveInterface* InInterface)
{
    TargetObject.SetObject(Cast<UObject>(InInterface));
    TargetObject.SetInterface(InInterface);
    InInterface->SetSelectMode(true); // 選択状態を付与
}

// 選択中のターゲットをリセット
void UColorTargetRegistry::ResetColorTarget()
{
    TargetObject->SetSelectMode(false);
}

// =======================
// ターゲット登録処理
// =======================

// 指定モードに新しいターゲットを登録
void UColorTargetRegistry::RegisterTarget(EColorTargetType Mode, TScriptInterface<IColorReactiveInterface> Target)
{
    if (!Target)
        return;

    FColorTargetInstanceArray& TargetArray = ColorResponseTargets.FindOrAdd(Mode);
    if (!TargetArray.Instances.Contains(Target))
    {
        TargetArray.Instances.Add(Target);
    }
}

// =======================
// ターゲット通知処理
// =======================

// 指定モードの全ターゲットに色を通知
void UColorTargetRegistry::NotifyTargets(EColorTargetType Mode, const FLinearColor& Color, FEffectMatchResult effect)
{
    if (FColorTargetInstanceArray* TargetArray = ColorResponseTargets.Find(Mode))
    {
        for (const TScriptInterface<IColorReactiveInterface>& Target : TargetArray->Instances)
        {
            if (Target)
            {
                // ターゲットの反応関数を呼び出す
                Target->ColorAction(Color, effect);
            }
        }
    }
}

// =======================
// ポストプロセスの初期化
// =======================

void UColorTargetRegistry::InitializePostEffect()
{
    // シーン内の PostProcessVolume を検索
    TArray<AActor*> FoundVolumes;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), APostProcessVolume::StaticClass(), FoundVolumes);

    if (FoundVolumes.Num() < 0)
        return;

    APostProcessVolume* PostProcessVolume = Cast<APostProcessVolume>(FoundVolumes[0]);

    if (PostProcessVolume && PostProcessMaterial)
    {
        // 動的マテリアルを作成して PostProcessVolume に適用
        PostProcessMID = UMaterialInstanceDynamic::Create(PostProcessMaterial, this);
        PostProcessVolume->Settings.WeightedBlendables.Array.Add(FWeightedBlendable(1.0f, PostProcessMID));
    }
}

// =======================
// ポストプロセスカラー取得
// =======================

FLinearColor UColorTargetRegistry::GetPostProcessColor() const
{
    if (!PostProcessMID)
        return FLinearColor::Black;  // 初期値がなければ黒を返す

    FLinearColor CurrentColor;
    if (PostProcessMID->GetVectorParameterValue(FName("FilterColor"), CurrentColor))
    {
        return CurrentColor;
    }
    else
    {
        // パラメータが存在しない場合は黒を返す
        return FLinearColor::Black;
    }
}

