// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DataContainer/StageInfo.h"
#include "StageSelectWidget.generated.h"

/**
 * 
 */
class UStageCardWidget;

UCLASS()
class PACHIO_API UStageSelectWidget : public UUserWidget
{
	GENERATED_BODY()

public:
    virtual void NativeConstruct() override;

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stage")
    TArray<FStageInfo> StageList;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stage")
    TSubclassOf<UStageCardWidget> StageCardClass;

    UPROPERTY(meta = (BindWidget))
    class UHorizontalBox* CardBox;

    UPROPERTY(meta = (BindWidget))
    class UButton* PlayButton;

    int32 CurrentIndex = 0;

    void UpdateCards();
    void MoveSelection(int32 Offset);

    UFUNCTION()
    void OnPlayPressed();

    void MoveSelectionLeft();

    void MoveSelectionRight();

    // UStageSelectWidget メンバ変数
    TArray<FName> StageWidgetNames;
};
