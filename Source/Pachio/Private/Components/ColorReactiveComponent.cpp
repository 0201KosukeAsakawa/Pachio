// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/ColorReactiveComponent.h"

// Sets default values for this component's properties
UColorReactiveComponent::UColorReactiveComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

bool UColorReactiveComponent::IsColorMatch(const FLinearColor& FilterColor, const float Tolerance) const
{
	{
		return FMath::Abs(Color.R - FilterColor.R) <= Tolerance &&
			FMath::Abs(Color.G - FilterColor.G) <= Tolerance &&
			FMath::Abs(Color.B - FilterColor.B) <= Tolerance;
	}
}

void UColorReactiveComponent::OnColorMatched()
{
}

