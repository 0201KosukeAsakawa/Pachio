// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Objects/Color/ColorReactiveObject.h"
#include "DataContainer/EffectMatchResult.h"
#include "ColorReactiveSwitch.generated.h"

class UBoxComponent;

UCLASS()
class PACHIO_API AColorReactiveSwitch : public AColorReactiveObject
{
	GENERATED_BODY()
public:
	AColorReactiveSwitch();
	virtual void Init()override;
private:
	virtual void ColorAction(const FLinearColor InColor, FEffectMatchResult) override;
private:
	UPROPERTY(VisibleAnywhere)
	UBoxComponent* BoxComponent;

	UPROPERTY(EditAnywhere)
	EBuffEffect Second;

	FLinearColor SecondColor;
};
