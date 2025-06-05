// Fill out your copyright notice in the Description page of Project Settings.


#include "Manager/ColorManager.h"
#include "Components/ColorControllerComponent.h"
#include "Interface/ColorFilterInterface.h"
#include "UObject/UObjectGlobals.h" 
#include "Kismet/GameplayStatics.h"
#include "Manager/LevelManager.h"
#include "UI/UIManager.h"
#include "Engine/PostProcessVolume.h"
#include "Blueprint/UserWidget.h"

void UColorManager::InitializeTargets()
{
    ColorTargets.Empty();

    for (auto& Pair : ColorTargetsClass)
    {
        EColorMode ModeKey = Pair.Key;
        const FColorTargetArray& ClassArray = Pair.Value;

        FColorTargetInstanceArray& InstanceArray = ColorTargets.FindOrAdd(ModeKey);
        InstanceArray.Instances.Empty();

        for (TSubclassOf<UObject> TargetClass : ClassArray.Targets)
        {
            if (TargetClass)
            {
                UObject* NewObj = NewObject<UObject>(this, TargetClass);
                if (NewObj && NewObj->GetClass()->ImplementsInterface(UColorFilterInterface::StaticClass()))
                {
                    TScriptInterface<IColorFilterInterface> InterfaceObj;
                    InterfaceObj.SetObject(NewObj);
                    InterfaceObj.SetInterface(Cast<IColorFilterInterface>(NewObj));
                    InstanceArray.Instances.Add(InterfaceObj);
                }
            }
        }
    }

    // ActiveLayerTarget�̏�����
    ActiveLayerTarget = nullptr;

    APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    if (PlayerPawn)
    {
        UColorControllerComponent* ColorController = PlayerPawn->FindComponentByClass<UColorControllerComponent>();
        if (ColorController)
        {
            ColorController->OnColorChanged.AddDynamic(this, &UColorManager::ApplyColor);
        }
    }

    // APostProcessVolume を自動取得
    TArray<AActor*> FoundVolumes;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), APostProcessVolume::StaticClass(), FoundVolumes);

    if (FoundVolumes.Num() > 0)
    {
        APostProcessVolume* PostProcessVolume = Cast<APostProcessVolume>(FoundVolumes[0]);

        if (PostProcessVolume && PostProcessMaterial)
        {
            PostProcessMID = UMaterialInstanceDynamic::Create(PostProcessMaterial, this);
            PostProcessVolume->Settings.WeightedBlendables.Array.Add(FWeightedBlendable(1.0f, PostProcessMID));
        }
    }
}

void UColorManager::ApplyColor(FLinearColor NewColor)
{
    switch (Mode)
    {
    case EColorMode::Layer:
    {
        if (PostProcessMID)
        {
            PostProcessMID->SetVectorParameterValue(TEXT("FilterColor"), NewColor);
        }
        break;
    }
    case EColorMode::Object:
    case EColorMode::Background:
    {
    
        break;
    }
    default:
        break;
    }
    if (FColorTargetInstanceArray* TargetArray = ColorTargets.Find(EColorMode::Object))
    {
        for (const TScriptInterface<IColorFilterInterface>& Target : TargetArray->Instances)
        {
            if (Target)
            {
                //ここでよぶ
                Target->ColorAction(NewColor);
            }
        }
    }
    CurrentColor = NewColor;
}


void UColorManager::RegisterTarget(EColorMode mode, TScriptInterface<IColorFilterInterface> Target)
{
    if (!Target) return;

    FColorTargetInstanceArray& TargetArray = ColorTargets.FindOrAdd(mode);
    if (!TargetArray.Instances.Contains(Target))
    {
        TargetArray.Instances.Add(Target);
    }
}
