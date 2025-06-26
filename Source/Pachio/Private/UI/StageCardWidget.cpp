// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/StageCardWidget.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/Border.h"

void UStageCardWidget::SetStageInfo(const FStageInfo& InInfo, bool bIsSelected)
{
    if (TitleText)
        TitleText->SetText(FText::FromString(InInfo.Title));

    if (JacketImage && InInfo.JacketImage)
        JacketImage->SetBrushFromTexture(InInfo.JacketImage);

    if (RankText)
    {
        FString RankStr = (InInfo.ClearRank == EStageRank::None) ? "-" : UEnum::GetValueAsString(InInfo.ClearRank);
        RankText->SetText(FText::FromString(RankStr));
    }

    if (SelectionBorder)
        SelectionBorder->SetBrushColor(bIsSelected ? FLinearColor::Yellow : FLinearColor::Transparent);
}