// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/ColorReactiveComponent.h"
#include "ColorTriggerStopComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PACHIO_API UColorTriggerStopComponent : public UColorReactiveComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UColorTriggerStopComponent();
private:
	void OnColorMatched(const FLinearColor& FilterColor)override;
	void OnColorMismatched(const FLinearColor& FilterColor)override;
	void UpdateAppearanceByColorDistance(const FLinearColor& FilterColor)override;
};
