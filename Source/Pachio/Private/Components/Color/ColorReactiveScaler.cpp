// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/Color/ColorReactiveScaler.h"

// Sets default values for this component's properties
UColorReactiveScaler::UColorReactiveScaler()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

void UColorReactiveScaler::Initialize()
{
	UObjectColorComponent::Initialize();
	DefaultScalse = GetOwner()->GetActorScale();
}

void UColorReactiveScaler::ApplyColorWithMatching(const FLinearColor& NewColor)
{
	SetColor(NewColor);
	const FLinearColor& Blue = UColorUtilityLibrary::FromHue(240);
	float distance = UColorUtilityLibrary::GetColorRatio(NewColor, Blue);
	if (0.3 < distance)
		GetOwner()->SetActorScale3D(DefaultScalse * distance);
}