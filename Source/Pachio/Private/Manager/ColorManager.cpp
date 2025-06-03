// Fill out your copyright notice in the Description page of Project Settings.


#include "Manager/ColorManager.h"
#include "Components/ColorControllerComponent.h"
#include "Interface/ColorFilterInterface.h"
#include "UObject/UObjectGlobals.h" 

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

    // ActiveLayerTargetÇÃèâä˙âª
    ActiveLayerTarget = nullptr;
    if (ActiveLayerTargetClass)
    {
        UObject* NewObj = NewObject<UObject>(this, ActiveLayerTargetClass);
        if (NewObj && NewObj->GetClass()->ImplementsInterface(UColorFilterInterface::StaticClass()))
        {
            ActiveLayerTarget.SetObject(NewObj);
            ActiveLayerTarget.SetInterface(Cast<IColorFilterInterface>(NewObj));
        }
    }
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
