// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/ColorControllerComponent.h"

// Sets default values for this component's properties
UColorControllerComponent::UColorControllerComponent()
{

}

void UColorControllerComponent::AdjustColor(EColorChannel Channel, float Delta)
{
    switch (Channel)
    {
    case EColorChannel::R:
        CurrentColor.R = FMath::Clamp(CurrentColor.R + Delta, 0.f, 1.f);
        break;
    case EColorChannel::G:
        CurrentColor.G = FMath::Clamp(CurrentColor.G + Delta, 0.f, 1.f);
        break;
    case EColorChannel::B:
        CurrentColor.B = FMath::Clamp(CurrentColor.B + Delta, 0.f, 1.f);
        break;
    }

    // デリゲートを呼んで通知
    OnColorChanged.Broadcast(CurrentColor);
}


