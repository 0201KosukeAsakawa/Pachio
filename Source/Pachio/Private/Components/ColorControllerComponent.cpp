// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/ColorControllerComponent.h"

// Sets default values for this component's properties
UColorControllerComponent::UColorControllerComponent()
{

}

void UColorControllerComponent::AdjustColor(EColorChannel Channel, float Delta)
{
    //if (Channel == EColorChannel::Hue) // 色相を変更
    {
        // 色相を変更
        float Hue = CurrentColor.R; // 現在の色相（CurrentColor.Rは色相として使われていると仮定）

        // 色相を変更
        Hue += Delta; // Deltaで色相を変更
        if (Hue > 1.f) Hue -= 1.f; // 1を超えたら0に戻す（色相環を一周させる）
        if (Hue < 0.f) Hue += 1.f; // 0未満にならないように調整

        // 新しい色相を設定（HSV→RGB変換）
        CurrentColor = FLinearColor::MakeFromHSV8(Hue * 360.0f, CurrentColor.G, CurrentColor.B); // Deltaで調整した色相を反映

        // 色を変更した後、デリゲートを呼び出す
        OnColorChanged.Broadcast(CurrentColor);
    }
}

