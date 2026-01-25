// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MovieWidget.generated.h"

/**
 * @brief ゲーム内ムービー再生用のUIウィジェット
 * Blueprint側でムービー再生・停止を実装する
 */
UCLASS()
class PACHIO_API UMovieWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    /**
     * @brief ムービーを再生する（Blueprintで実装）
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Movie")
    void PlayMovie();

    /**
     * @brief ムービーを停止する（Blueprintで実装）
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Movie")
    void StopMovie();
};
