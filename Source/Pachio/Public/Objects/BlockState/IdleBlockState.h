// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BlockState.h"
#include "IdleBlockState.generated.h"

struct FAttackData;
/**
 * 
 */
UCLASS()
class PACHIO_API UIdleBlockState : public UBlockState
{
	GENERATED_BODY()
public:
	virtual bool OnEnter(AActor*, UWorld*)override;
	virtual bool OnUpdate(AActor*)override;
	virtual bool OnExit(AActor*)override;
	virtual bool OnHit(FVector, FAttackData)override;
};
