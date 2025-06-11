// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Objects/ColorReactiveObject.h"
#include "Interface/ColorFilterInterface.h"
#include "Interface/ColorReactionConfigInterface.h"
#include "ColorMuchTrigger.generated.h"

class UColorReactiveComponent;

UCLASS()
class PACHIO_API AColorMuchTrigger : public AColorReactiveObject
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AColorMuchTrigger();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	virtual void ColorAction(FLinearColor InColor) override;

	// サブクラスからも使えるカラー反映関数
	void ApplyColorToMaterial(FLinearColor InColor);
};
