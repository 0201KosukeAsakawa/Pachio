// Fill out your copyright notice in the Description page of Project Settings.


#include "Manager/ColorManager.h"
#include "Components/ColorControllerComponent.h"
#include "Interface/ColorFilterInterface.h"

void UColorManager::Init()
{

    // プレイヤーキャラクターを取得
    if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
    {
        if (APawn* Pawn = PC->GetPawn())
        {
            if (UColorControllerComponent* Controller = Pawn->FindComponentByClass<UColorControllerComponent>())
            {
                // バインド（ApplyColor の引数は FLinearColor である必要がある）
                Controller->OnColorChanged.AddDynamic(this, &UColorManager::ApplyColor);
            }
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
        for (TScriptInterface<IColorFilterInterface> Target : ColorTargets.FindRef(Mode))
        {
            if (Target)
            {
                Target->SetColor(NewColor);
            }
        }
        break;
    }
}


void UColorManager::RegisterTarget(EColorMode mode, TScriptInterface<IColorFilterInterface> Target)
{
    if (!Target) return;

    TArray<TScriptInterface<IColorFilterInterface>>& Targets = ColorTargets.FindOrAdd(mode);

    if (!Targets.Contains(Target))
    {
        Targets.Add(Target);
    }
}

void UColorManager::SetMode(EColorMode nextMode)
{
    Mode = nextMode;
}
