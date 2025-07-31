// Fill out your copyright notice in the Description page of Project Settings.


#include "Logic/ColorManager/ColorTargetRegistry.h"
#include "Interface/ColorFilterInterface.h"
#include "Kismet/GameplayStatics.h"

void UColorTargetRegistry::ApplyColor(FLinearColor NewColor, EColorTargetType Mode)
{
    switch (Mode)
    {
    case EColorTargetType::WorldColor:
        if (PostProcessMID)
        {
            // ポストプロセスマテリアルに色を適用
            PostProcessMID->SetVectorParameterValue(TEXT("FilterColor"), NewColor);
        }
        // 指定されたモードのターゲットに通知
        NotifyTargets(Mode, NewColor);
        // 常時反応するターゲット（例：UIなど）に通知
        NotifyTargets(EColorTargetType::Responders, NewColor);
        break;

    case EColorTargetType::ObjectColor:
        // 指定されたモードのターゲットに通知
        if (!TargetObject)
            return;
        TargetObject->SetColor(NewColor);
        break;

    default:
        break;
    }
}

void UColorTargetRegistry::ColorEvent(FName EventID,FLinearColor NewColor)
{
    if (!ColorResponseTargets.Contains(EColorTargetType::Event))
    {
        // EColorTargetType::Eventのキーが存在しなければ処理終了
        return;
    }

    auto& Instances = ColorResponseTargets[EColorTargetType::Event].Instances;

    if (Instances.Num() == 0)
        return;

    for (auto& TargetInstance : Instances)
    {
        if (TargetInstance->GetColorEventID() != EventID)
            continue;

        TargetInstance->ColorAction(NewColor);
    }
}

void UColorTargetRegistry::SetColorTarget(IColorReactiveInterface* InInterface)
{
    TargetObject.SetObject(Cast<UObject>(InInterface));
    TargetObject.SetInterface(InInterface);
    InInterface->SetSelectMode(true);
}

void UColorTargetRegistry::ResetColorTarget()
{   
    TargetObject->SetSelectMode(false);
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

FLinearColor UColorTargetRegistry::GetPostProcessColor() const
{
    if (!PostProcessMID) return FLinearColor::Black;  // あるいはデフォルト色

    FLinearColor CurrentColor;
    if (PostProcessMID->GetVectorParameterValue(FName("FilterColor"), CurrentColor))
    {
        return CurrentColor;
    }
    else
    {
        // パラメータがなければ黒とか適当な色を返す
        return FLinearColor::Black;
    }
}

