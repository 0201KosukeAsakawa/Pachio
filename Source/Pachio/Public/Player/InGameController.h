// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InGameController.generated.h"

/**
 * 
 */
UCLASS()
class PACHIO_API AInGameController : public APlayerController
{
	GENERATED_BODY()

protected:
    virtual void SetupInputComponent() override;

private:
    void TogglePossession();
    AActor* FindPossessableObject();

private:
    UPROPERTY()
    APawn* OriginalPawn = nullptr;
    bool bIsPossessing = false;

    UPROPERTY(EditDefaultsOnly)
    float DetectionDistance = 300.f;

    UPROPERTY(EditDefaultsOnly)
    FVector BoxHalfSize = FVector(50.f, 50.f, 50.f);
};
