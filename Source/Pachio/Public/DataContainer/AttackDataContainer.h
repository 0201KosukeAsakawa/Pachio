// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Attack/AttackStrategy.h"
#include "AttackDataContainer.generated.h"


class UAttackComponent;
/**
 * 
 */
UCLASS(Blueprintable)
class PACHIO_API UAttackDataContainer : public UObject
{
	GENERATED_BODY()

public:

    // 攻撃タイプ → 戦略クラス
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack")
    TMap<FName, TSubclassOf<UAttackStrategy>> AttackStrategyMap;

    // 攻撃タイプ → 戦略クラス
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack")
    TMap<FName, TSubclassOf<UAttackComponent>> AttckComponentMap;

    // 戦略をインスタンス化して返す
    UAttackStrategy* CreateStrategy(UObject*, FName ) const;

    UAttackComponent* GenerateAttackComponent(AActor* Owner, FName Type)const;
};
