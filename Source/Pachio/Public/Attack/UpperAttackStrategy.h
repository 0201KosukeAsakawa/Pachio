// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Attack/AttackStrategy.h"
#include "UpperAttackStrategy.generated.h"

/**
 * 
 */
UCLASS()
class PACHIO_API UUpperAttackStrategy : public UAttackStrategy
{
	GENERATED_BODY()
public:
	virtual bool ExecuteEffect(AActor* Attacker, AActor* Target, FAttackData, float FinalDamage = 0)override;
};
