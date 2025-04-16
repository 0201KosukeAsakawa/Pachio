// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Attack/AttackStrategy.h"
#include "StompAttackStrategy.generated.h"

/**
 * 
 */
UCLASS()
class PACHIO_API UStompAttackStrategy : public UAttackStrategy
{
	GENERATED_BODY()
public:
	virtual void ExecuteEffect(AActor*, AActor*,float FinalDamage = 0)override;
};
