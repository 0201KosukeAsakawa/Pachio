// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interface/ColorFilterInterface.h"
#include "Blueprint/UserWidget.h"
#include "ColorLens.generated.h"

/**
 * 
 */
class UImage;

UCLASS()
class PACHIO_API UColorLens : public UUserWidget,public IColorReactiveInterface
{
	GENERATED_BODY()
public:
	void NativeConstruct();
private:
	void ColorAction(FLinearColor)override;
protected:
	//
	UPROPERTY(meta = (BindWidget))
	UImage* FilterColorImage;
};
