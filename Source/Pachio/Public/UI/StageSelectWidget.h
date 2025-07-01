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
class UCanvasPanel;

UCLASS()
class PACHIO_API UStageSelectWidget : public UUserWidget
{
	GENERATED_BODY()

public:
    virtual void NativeConstruct() override;

protected:
    TArray<FStageInfo> StageList;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stage")
    TSubclassOf<UStageCardWidget> StageCardClass;
    // ヘッダファイルなどで宣言
    UPROPERTY()
    TArray<UStageCardWidget*> StageWidgets;

    UPROPERTY(meta = (BindWidget))
    class UButton* PlayButton;

    UPROPERTY(meta = (BindWidget))
    UCanvasPanel* CardCanvas;

    int32 CurrentIndex = 0;

    void GenerateStageCardsFromSave();
    void UpdateCardPositions();
    void MoveSelection(int32 Offset);

    UFUNCTION()
    void OnPlayPressed();

    void MoveSelectionLeft();

    void MoveSelectionRight();

    // UStageSelectWidget メンバ変数
    TArray<FName> StageWidgetNames;
};
