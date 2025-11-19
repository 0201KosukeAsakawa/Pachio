// Fill out your copyright notice in the Description page of Project Settings.


#include "Logic/ColorManager/ColorTargetRegistry.h"
#include "Logic/ColorManager/EffectColorRegistry.h"
#include "Interface/ColorFilterInterface.h"
#include "Kismet/GameplayStatics.h"


// =======================
// 色の適用処理
// =======================

void UColorTargetRegistry::ApplyColor(FLinearColor NewColor)
{
    if (PostProcessMID)
    {
        // ポストプロセスマテリアルに色を適用（画面全体のカラー演出）
        PostProcessMID->SetVectorParameterValue(TEXT("FilterColor"),NewColor);
    }
    // 指定されたモードのターゲットに通知
    NotifyTargets(NewColor);
    // 常時反応するターゲット（例：UIなど Responders）にも通知
    NotifyTargets(NewColor);

    // 色適用イベントをブロードキャスト
    OnColorApplied.Broadcast(NewColor);
}

// =======================
// イベント用の色適用処理
// =======================

void UColorTargetRegistry::ColorEvent(FName EventID, FLinearColor NewColor)
{   
    auto& instances = Instances;
    if (Instances.Num() == 0)
        return;

    // EventID が一致するターゲットにのみ通知
    for (auto& TargetInstance : instances)
    {
        if (TargetInstance->GetColorEventID() != EventID)
            continue;

        TargetInstance->ApplyColorWithMatching(NewColor);
    }
}

// =======================
// ターゲット登録処理
// =======================

// 新しいターゲットを登録
void UColorTargetRegistry::RegisterTarget(TScriptInterface<IColorReactiveInterface> Target)
{
    if (!Target)
        return;

    if (!Instances.Contains(Target))
    {
        Instances.Add(Target);
    }
}

// =======================
// ターゲット通知処理
// =======================

//全ターゲットに色を通知
void UColorTargetRegistry::NotifyTargets(const FLinearColor& Color)
{
    for (const TScriptInterface<IColorReactiveInterface>& Target : Instances)
    {
        if (Target)
        {
            // ターゲットの反応関数を呼び出す
            Target->ApplyColorWithMatching(Color);
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

