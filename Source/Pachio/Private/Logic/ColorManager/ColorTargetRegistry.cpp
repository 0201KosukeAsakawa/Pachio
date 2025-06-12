// Fill out your copyright notice in the Description page of Project Settings.


#include "Logic/ColorManager/ColorTargetRegistry.h"
#include "Interface/ColorFilterInterface.h"
#include "Kismet/GameplayStatics.h"

void UColorTargetRegistry::ApplyColor(FLinearColor NewColor, EColorTargetType Mode)
{
    switch (Mode)
    {
    case EColorTargetType::Layer:
        if (PostProcessMID)
        {
            // ポストプロセスマテリアルに色を適用
            PostProcessMID->SetVectorParameterValue(TEXT("FilterColor"), NewColor);
        }
        // 常時反応するターゲット（例：UIなど）に通知
        NotifyTargets(EColorTargetType::Responders, NewColor);
        break;

    case EColorTargetType::Object:
    case EColorTargetType::Background:
        // 指定されたモードのターゲットに通知
        NotifyTargets(Mode, NewColor);
        break;

    default:
        break;
    }
}

void UColorTargetRegistry::ColorEvent(FName EventID)
{
    for (auto& TargetInstance : ColorResponseTargets[EColorTargetType::Event].Instances)
    {
        if (TargetInstance->GetColorEventID() != EventID)
            return;

        TargetInstance->ColorAction();
    }
}


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

void UColorTargetRegistry::NotifyTargets(EColorTargetType Mode, const FLinearColor& Color)
{
    if (FColorTargetInstanceArray* TargetArray = ColorResponseTargets.Find(Mode))
    {
        for (const TScriptInterface<IColorReactiveInterface>& Target : TargetArray->Instances)
        {
            if (Target)
            {
                // ターゲットの反応関数を呼び出す
                Target->ColorAction(Color);
            }
        }
    }
}

void UColorTargetRegistry::InitializePostEffect()
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

