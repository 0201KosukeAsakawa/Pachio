// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/ColorReactiveComponent.h"

// Sets default values for this component's properties
UColorReactiveComponent::UColorReactiveComponent()
{

}

void UColorReactiveComponent::SetMyColor(const FLinearColor& FilterColor)
{
	Color = FilterColor;
}

void UColorReactiveComponent::CheckColorMatch(const FLinearColor& FilterColor)
{
    // 色のマッチング判定は保持しておきつつ

    bool bMatch = IsColorMatch(FilterColor);

    // 透明度・表示の更新は常に行う（色の近さに応じて）
    //UpdateAppearanceByColorDistance(FilterColor);

    if (bMatch)
    {
        OnColorMatched(FilterColor);
    }
    else
    {
        OnColorMismatched(FilterColor);
    }
}

bool UColorReactiveComponent::IsColorMatch(const FLinearColor& FilterColor, const float Tolerance) const
{
	return FMath::Abs(Color.R - FilterColor.R) <= Tolerance &&
		FMath::Abs(Color.G - FilterColor.G) <= Tolerance &&
		FMath::Abs(Color.B - FilterColor.B) <= Tolerance;
}

void UColorReactiveComponent::OnColorMatched(const FLinearColor& FilterColor)
{
}

void UColorReactiveComponent::OnColorMismatched(const FLinearColor& FilterColor)
{
}

void UColorReactiveComponent::UpdateAppearanceByColorDistance(const FLinearColor& FilterColor)
{
}


