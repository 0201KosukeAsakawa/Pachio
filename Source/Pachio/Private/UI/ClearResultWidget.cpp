// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ClearResultWidget.h"

void UClearResultWidget::SetupClearResult(float InClearTime, EStageRank InScore)
{
    CachedClearTime = InClearTime;
    CachedRank = InScore;
}

FText UClearResultWidget::GetClearTimeText() const
{
    return FText::FromString(FString::Printf(TEXT("%.1f"), CachedClearTime));
}


FText UClearResultWidget::GetRankText() const
{
    FString RankText;
    switch (CachedRank)
    {
    case EStageRank::S:
        RankText = TEXT("S");
        break;
    case EStageRank::A:
        RankText = TEXT("A");
        break;
    case EStageRank::B:
    default:
        RankText = TEXT("B");
        break;
    }
    return FText::FromString(RankText);
}

void UClearResultWidget::NativeConstruct()
{
    Super::NativeConstruct();
    PlayResultAnimation();
}

void UClearResultWidget::PlayResultAnimation()
{
    if (FadeInAnimation)
    {
        PlayAnimation(FadeInAnimation);
    }
}