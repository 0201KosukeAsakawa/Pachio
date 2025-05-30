// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ColorLens.generated.h"

/**
 * 
 */
class UImage;

UCLASS()
class PACHIO_API UColorLens : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	void UpdateFilterColor(const FLinearColor& NewColor);

protected:
	// 例えば画面の色を変えるためのUIパーツ
	UPROPERTY(meta = (BindWidget))
	UImage* FilterColorImage;
};
