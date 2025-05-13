// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ScoreManager.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class PACHIO_API UScoreManager : public UObject
{
	GENERATED_BODY()
	

public:
	UScoreManager();

	void AddCoin(int);
	void AddScore(int);

	//スコアの取得
	UFUNCTION(BlueprintCallable)
	inline int GetGameScore()const { return GameScore; }
	//コインの取得
	UFUNCTION(BlueprintCallable)
	inline int GetCoin()const { return Coin; }

private:
	//ゲーム内Score
	int GameScore;
	//コイン取得数
	int Coin;
	//残機
	int remaininglives;
};
