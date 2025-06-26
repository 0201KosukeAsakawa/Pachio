// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DataContainer/StageInfo.h"
#include "StageCardWidget.generated.h"

class UTextBlock;
class UImage;
class UBorder;


UCLASS()
class PACHIO_API UStageCardWidget : public UUserWidget
{
	GENERATED_BODY()

public:
    void SetStageInfo(const FStageInfo& InInfo, bool bIsSelected);

protected:
    UPROPERTY(meta = (BindWidget))
     UTextBlock* TitleText;

    UPROPERTY(meta = (BindWidget))
     UImage* JacketImage;

    UPROPERTY(meta = (BindWidget))
     UTextBlock* RankText;

    UPROPERTY(meta = (BindWidget))
     UBorder* SelectionBorder;
};
