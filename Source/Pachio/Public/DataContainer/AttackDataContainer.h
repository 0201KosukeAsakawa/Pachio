// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Attack/AttackStrategy.h"
#include "AttackDataContainer.generated.h"

/**
 * 
 */
UCLASS()
class PACHIO_API UAttackDataContainer : public UObject
{
	GENERATED_BODY()

public:

    // 攻撃タイプ → 戦略クラス
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack")
    TMap<FString, TSubclassOf<UAttackStrategy>> AttackStrategyMap;

    // 戦略をインスタンス化して返す
  //z  UFUNCTION(BlueprintCallable, Category = "Attack")
    UAttackStrategy* CreateStrategy(UObject*, FString ) const;
};
