// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/ColorReactiveComponent.h"

// Sets default values for this component's properties
UColorReactiveComponent::UColorReactiveComponent()
{

}

void UColorReactiveComponent::Init(UMeshComponent* mesh)
{
    // マテリアルの色を変更
    UMaterialInstanceDynamic* DynMaterial = mesh->CreateAndSetMaterialInstanceDynamic(0);
    if (DynMaterial)
    {
        DynMaterial->SetVectorParameterValue(FName("BaseColor"), Color);
    }
}

void UColorReactiveComponent::SetMyColor(const FLinearColor& FilterColor)
{
	Color = FilterColor;
}

void UColorReactiveComponent::CheckColorMatch(const FLinearColor& FilterColor)
{
    // 色のマッチング判定は保持しておきつつ

    bool bMatch = IsColorMatch(FilterColor);

    if (bMatch)
    {
        UE_LOG(LogTemp, Log, TEXT("[%s]TRUE"), *GetOwner()->GetName());
        OnColorMatched(FilterColor);
    }
    else
    {
        UE_LOG(LogTemp, Log, TEXT("[%s] FLASE"), *GetOwner()->GetName());
        OnColorMismatched(FilterColor);
    }
}

bool UColorReactiveComponent::IsColorMatch(const FLinearColor& FilterColor, const float Tolerance) const
{
    float dR = Color.R - FilterColor.R;
    float dG = Color.G - FilterColor.G;
    float dB = Color.B - FilterColor.B;

    // 人間の目に近い重み付き色差（輝度ベース）
    float ColorDifference = 0.299f * dR * dR + 0.587f * dG * dG + 0.114f * dB * dB;

    return ColorDifference <= Tolerance * Tolerance;
}


void UColorReactiveComponent::OnColorMatched(const FLinearColor& FilterColor)
{
}

void UColorReactiveComponent::OnColorMismatched(const FLinearColor& FilterColor)
{
}


