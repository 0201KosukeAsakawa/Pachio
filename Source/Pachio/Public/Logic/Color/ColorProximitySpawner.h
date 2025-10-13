// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Color/ColorReactiveComponent.h"
#include "ColorProximitySpawner.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PACHIO_API UColorProximitySpawner : public UColorReactiveComponent
{
	GENERATED_BODY()
public:
	UColorProximitySpawner();

private:
	void Initialize(const FLinearColor& InitialColor, bool bVariable , AActor* owner)override;

	void OffMesh();
	void OnMesh();

	bool OnColorMatched(const FLinearColor& FilterColor)override;
	bool OnColorMismatched(const FLinearColor& FilterColor)override;
};
