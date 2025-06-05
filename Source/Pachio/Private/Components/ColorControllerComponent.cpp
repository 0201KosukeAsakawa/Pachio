// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/ColorControllerComponent.h"

// Sets default values for this component's properties
UColorControllerComponent::UColorControllerComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UColorControllerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    /*float decreaseSpeed = 0.1f;
    AdjustColor(EColorChannel::R, -decreaseSpeed * DeltaTime);*/
}

void UColorControllerComponent::AdjustColor(EColorChannel Channel, float Delta)
{
    // RGB → HSV に変換
    FLinearColor HSV = CurrentColor.LinearRGBToHSV();    
    
    float Hue = HSV.R;  // 0〜360
    float Saturation = HSV.G;
    float Value = HSV.B;

    // Hue を調整
    Hue += Delta * 360.0f;

    if (Hue > 360.f) 
        Hue -= 360.f;
    if (Hue < 0.f) 
        Hue += 360.f;

    // HSV → RGB に変換
    FLinearColor NewColor = FLinearColor(Hue, Saturation, Value).HSVToLinearRGB();
    CurrentColor.R = NewColor.R;
    CurrentColor.G = NewColor.G;
    CurrentColor.B = NewColor.B;

    HSV = CurrentColor.LinearRGBToHSV();
    UE_LOG(LogTemp, Log, TEXT("Begin Color changed:RGB: R=%.3f G=%.3f B=%.3f"), CurrentColor.R, CurrentColor.G, CurrentColor.B);
    UE_LOG(LogTemp, Log, TEXT("End Color changed:RGB: R=%.3f G=%.3f B=%.3f"), NewColor.R, NewColor.G, NewColor.B);

    // デリゲートを通知
    OnColorChanged.Broadcast(CurrentColor);
}


