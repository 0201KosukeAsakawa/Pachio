// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "DataContainer/StageInfo.h"
#include "ScoreManager.generated.h"



UCLASS(Blueprintable)
class PACHIO_API UScoreManager : public UObject
{
	GENERATED_BODY()
	

public:
	UScoreManager();

	void AddCoin(int);
	void AddScore(int);
	void Init();
	EStageRank EvaluateClearRank(UWorld* World);
	UFUNCTION(BlueprintCallable, Category = "UIManager")
	inline int GetTime()const { return InGameTimer; }
	//スコアの取得
	UFUNCTION(BlueprintCallable)
	inline int GetGameScore()const { return GameScore; }
	//コインの取得
	UFUNCTION(BlueprintCallable)
	inline int GetCoin()const { return Coin; }
	void CountDown() { ++InGameTimer; }
private:
	//ゲーム内Score
	int GameScore;
	//コイン取得数
	int Coin;
	UPROPERTY(EditAnywhere)
	float InGameTimer = 0;
	UPROPERTY(EditAnywhere)
	int SRankTime;
	UPROPERTY(EditAnywhere)
	int ARankTime;
	FTimerHandle CountTimerHandle;
};
