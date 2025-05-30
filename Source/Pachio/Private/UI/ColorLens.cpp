// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ColorLens.h"
#include "Components/Image.h"

void UColorLens::UpdateFilterColor(const FLinearColor& NewColor)
{
    if (!FilterColorImage)
        return;

    // 現在の色を取得
    FLinearColor CurrentColor = FilterColorImage->ColorAndOpacity;

    // RGB は新しい色に更新し、Alpha は既存値を維持
    FLinearColor CombinedColor = FLinearColor(
        NewColor.R,
        NewColor.G,
        NewColor.B,
        CurrentColor.A // Alphaだけ元のまま
    );

    FilterColorImage->SetColorAndOpacity(CombinedColor);

}