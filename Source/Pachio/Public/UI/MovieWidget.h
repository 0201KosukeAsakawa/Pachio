// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MovieWidget.generated.h"

/**
 * 
 */
UCLASS()
class PACHIO_API UMovieWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintImplementableEvent, Category = "Movie")
    void PlayMovie();

    UFUNCTION(BlueprintImplementableEvent, Category = "Movie")
    void StopMovie();
};