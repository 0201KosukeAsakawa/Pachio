// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BlockState.h"
#include "BlockDepletedState.generated.h"

/**
 * 
 */

struct FAttackData;

UCLASS(Blueprintable)
class PACHIO_API UBlockDepletedState : public UBlockState
{
	GENERATED_BODY()
public:
	bool OnEnter(AActor*, UWorld*)override;
	bool OnUpdate(AActor*)override;
	bool OnExit(AActor*)override;
	bool OnHit(FVector, FAttackData)override;
};
