// Fill out your copyright notice in the Description page of Project Settings.


#include "Manager/ColorManager.h"
#include "Manager/LevelManager.h"
#include "Components/ColorControllerComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Interface/ColorFilterInterface.h"
#include "UObject/UObjectGlobals.h"
#include "UI/UIManager.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/PostProcessVolume.h"
#include "Blueprint/UserWidget.h"
#include "FunctionLibrary.h"
#include "DataContainer/ColorTargetType.h"


void UColorManager::InitializeTargets()
{
    ColorResponseTargets.Empty();

    for (auto& Pair : ColorTargetsClass)
    {
        EColorTargetType ModeKey = Pair.Key;
        const FColorTargetArray& ClassArray = Pair.Value;

        FColorTargetInstanceArray& InstanceArray = ColorResponseTargets.FindOrAdd(ModeKey);
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
    case EColorTargetType::Layer:
    {
        if (PostProcessMID)
        {
            PostProcessMID->SetVectorParameterValue(TEXT("FilterColor"), NewColor);
        }
        break;
    }
    case EColorTargetType::Object:
    case EColorTargetType::Background:
        //カラーに反応するオブジェクトに現在のカラーを通知
        if (FColorTargetInstanceArray* TargetArray = ColorResponseTargets.Find(Mode))
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
        //ここで種類に応じてカラー変更
        break;
    
    default:
        break;
    }
    //カラーに反応するオブジェクトに現在のカラーを通知
    if (FColorTargetInstanceArray* TargetArray = ColorResponseTargets.Find(EColorTargetType::Responders))
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
}


void UColorManager::RegisterTarget(EColorTargetType mode, TScriptInterface<IColorFilterInterface> Target)
{
    if (!Target) return;

    FColorTargetInstanceArray& TargetArray = ColorResponseTargets.FindOrAdd(mode);
    if (!TargetArray.Instances.Contains(Target))
    {
        TargetArray.Instances.Add(Target);
    }
}
