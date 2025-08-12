// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "DataContainer/EffectMatchResult.h"
#include "DataContainer/ColorTargetType.h"
#include "EffectColorMatcher.generated.h"

/**
 * 
 */
UCLASS()
class PACHIO_API UEffectColorMatcher : public UObject
{
	GENERATED_BODY()
	
public:
	UEffectColorMatcher();
	FEffectMatchResult GetClosestEffectByHue(const FLinearColor& InputColor);
	float GetColorDistanceRGB(const FLinearColor& A, const FLinearColor& B);
	FLinearColor GetEffectColor(EBuffEffect)const;
private:
	TMap<EBuffEffect, FLinearColor> EffectColorMap;


};
