// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Delegates/Delegate.h"
#include "FadeWidget.generated.h"


UCLASS()
class PACHIO_API UFadeWidget : public UUserWidget
{
    GENERATED_BODY()
public:
    FSimpleDelegate OnFadeFinished;

    // Blueprint 側には Duration だけ渡す
    UFUNCTION(BlueprintImplementableEvent, Category = "Fade")
    void PlayFadeBP(float Duration);

    // C++ 側から呼ぶラッパー
    void PlayFade(float Duration, FSimpleDelegate Delegate);


    UFUNCTION(BlueprintCallable, Category = "Fade")
    void FadeFinished();
};
