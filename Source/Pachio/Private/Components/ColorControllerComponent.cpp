// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/ColorControllerComponent.h"

// Sets default values for this component's properties
UColorControllerComponent::UColorControllerComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UColorControllerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{

}

void UColorControllerComponent::AdjustColor(EColorChannel Channel, float Delta)
{
    // RGB → HSV に変換
    FLinearColor HSV = CurrentColor.LinearRGBToHSV();

    float Hue = HSV.R;  // 0〜360
    float Saturation = HSV.G;
    float Value = HSV.B;

    // 彩度か明度がほぼ0なら少し上げて色相の変化が見えるようにする
    if (Saturation < KINDA_SMALL_NUMBER)
    {
        Saturation = 1.f;
    }
    if (Value < KINDA_SMALL_NUMBER)
    {
        Value = 1.f;
    }

    // Hue を調整
    Hue += Delta * 360.0f;

    if (Hue > 360.f)
        Hue -= 360.f;
    if (Hue < 0.f)
        Hue += 360.f;

    // HSV → RGB に変換
    FLinearColor NewColor = FLinearColor(Hue, Saturation, Value).HSVToLinearRGB();

    // 現在の色を更新（アルファも保持）
    CurrentColor.R = NewColor.R;
    CurrentColor.G = NewColor.G;
    CurrentColor.B = NewColor.B;

    UE_LOG(LogTemp, Log, TEXT("End Color changed:RGB: R=%.3f G=%.3f B=%.3f"), NewColor.R, NewColor.G, NewColor.B);

    // デリゲートを通知
    OnColorChanged.Broadcast(CurrentColor);
}

