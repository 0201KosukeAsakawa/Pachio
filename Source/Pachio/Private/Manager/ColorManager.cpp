// Fill out your copyright notice in the Description page of Project Settings.


#include "Manager/ColorManager.h"
#include "Components/ColorControllerComponent.h"
#include "Interface/ColorFilterInterface.h"
#include "UObject/UObjectGlobals.h" 
#include "Kismet/GameplayStatics.h"
#include "Manager/LevelManager.h"
#include "UI/UIManager.h"
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

    // ActiveLayerTarget‚Ì‰Šú‰»
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


    ALevelManager* al = ALevelManager::GetInstance(GetWorld());
    UUserWidget* widget = al->GetUIManager()->GetWidget(EWidgetCategory::Lens, "ColorLensWidget");
    if (!widget || !widget->GetClass()->ImplementsInterface(UColorFilterInterface::StaticClass()))
        return;
    ActiveLayerTarget.SetObject(widget);
    ActiveLayerTarget.SetInterface(Cast<IColorFilterInterface>(widget));
}

void UColorManager::ApplyColor(FLinearColor NewColor)
{
    switch (Mode)
    {
    case EColorMode::Layer:
        if (ActiveLayerTarget)
        {
            ActiveLayerTarget->SetColor(NewColor);
        }
        break;

    case EColorMode::Object:
    case EColorMode::Background:
        if (FColorTargetInstanceArray* TargetArray = ColorTargets.Find(Mode))
        {
            for (const TScriptInterface<IColorFilterInterface>& Target : TargetArray->Instances)
            {
                if (Target)
                {
                    Target->SetColor(NewColor);
                }
            }
        }
        break;

    default:
        break;
    }
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
