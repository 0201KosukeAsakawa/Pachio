// Fill out your copyright notice in the Description page of Project Settings.


#include "Manager/ScoreManager.h"
#include "Manager/LevelManager.h"

UScoreManager::UScoreManager()
	:GameScore(0)
	,Coin(0)
{
}

void UScoreManager::AddCoin(int coin)
{
	Coin += coin;
}

void UScoreManager::AddScore(int score)
{
	GameScore += score;
}

void UScoreManager::Init()
{
    // 1秒ごとに CountUp 関数を呼ぶ
    GetWorld()->GetTimerManager().SetTimer(CountTimerHandle, this, &UScoreManager::CountDown, 1.0f, true);
}

EStageRank UScoreManager::EvaluateClearRank(UWorld* World)
{
    if (!World) 
        return EStageRank::B;

    ALevelManager* LevelManager = ALevelManager::GetInstance(World);
    if (!LevelManager) 
        return EStageRank::B;

    int ClearTime = InGameTimer;

    if (ClearTime <= SRankTime)
    {
        return EStageRank::S;
    }
    else if (ClearTime <= ARankTime)
    {
        return EStageRank::A;
    }
    else
    {
        return EStageRank::B;
    }
}