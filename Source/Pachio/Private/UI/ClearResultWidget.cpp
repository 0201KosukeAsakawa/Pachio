// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ClearResultWidget.h"
#include "DataContainer/ClearScoreType.h"
void UClearResultWidget::SetupClearResult(float InClearTime, EClearScore InScore)
{
    CachedClearTime = InClearTime;
    CachedRank = InScore;
}

FText UClearResultWidget::GetClearTimeText() const
{
    return FText::FromString(FString::Printf(TEXT("%.2f"), CachedClearTime));
}


FText UClearResultWidget::GetRankText() const
{
    FString RankText;
    switch (CachedRank)
    {
    case EClearScore::S:
        RankText = TEXT("S");
        break;
    case EClearScore::A:
        RankText = TEXT("A");
        break;
    case EClearScore::B:
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