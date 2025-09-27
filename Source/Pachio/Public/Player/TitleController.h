// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TitleController.generated.h"

UENUM(BlueprintType)
enum class ETitleState : uint8
{
    Title,
    FadingToMovie,
    InMovie,
    FadingToTitle
};

UCLASS()
class PACHIO_API ATitleController : public APlayerController
{
    GENERATED_BODY()

public:
    ATitleController();

protected:
    virtual void SetupInputComponent() override;
    void OnAnyAxisInput(float Value);
    virtual void Tick(float DeltaSeconds) override;

    void OnAnyInput();
    void StartMovie();
    void EndMovie();

private:
    float IdleCounter;
    float IdleThreshold;

    ETitleState CurrentState;

    // フェード用Widget
    UPROPERTY(EditAnywhere, Category = "UI")
    TSubclassOf<class UUserWidget> FadeWidgetClass;

    UPROPERTY()
    class UFadeWidget* FadeWidget;

    // ムービーWidget
    UPROPERTY(EditAnywhere, Category = "UI")
    TSubclassOf<class UUserWidget> MovieWidgetClass;

    UPROPERTY()
    class UMovieWidget* MovieWidget;

    void ShowFade(float Duration, FSimpleDelegate OnFinished);
    void ShowMovie();
    void HideMovie();
};
