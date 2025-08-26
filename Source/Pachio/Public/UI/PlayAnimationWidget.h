// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayAnimationWidget.generated.h"

/**
 * 
 */
UCLASS()
class PACHIO_API UPlayAnimationWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void PlayAnimationByName(FName AnimName);
	void StopAnimationByName(FName AnimName);
	void StopAllAnimation();
};
