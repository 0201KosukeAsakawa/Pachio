// Fill out your copyright notice in the Description page of Project Settings.


#include "Manager/ScoreManager.h"

UScoreManager::UScoreManager()
	:GameScore(0)
	,Coin(0)
	, remaininglives(0)
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
