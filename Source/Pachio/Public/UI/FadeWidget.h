// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Delegates/DelegateCombinations.h"
#include "FadeWidget.generated.h"


UCLASS()
class PACHIO_API UFadeWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	// C++ 側で完了したら呼ぶための Delegate
	FSimpleDelegate OnFadeFinished;

	UFUNCTION(BlueprintImplementableEvent, Category = "Fade")
	void PlayFade(float Duration );

	UFUNCTION(BlueprintCallable, Category = "Fade")
	void FadeFinished() { OnFadeFinished.ExecuteIfBound(); }
};
