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
	bool OnEnter(ABaseBlock*, UWorld*, UBlockDataContainer*,const FString materialID = "None")override;
	bool OnUpdate(ABaseBlock*)override;
	bool OnExit(ABaseBlock*)override;
	bool OnHit(FAttackData, FVector)override;
};
