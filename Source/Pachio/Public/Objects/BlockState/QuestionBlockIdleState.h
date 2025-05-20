// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BlockState.h"
#include "QuestionBlockIdleState.generated.h"

struct FAttackData;
/**
 * 
 */
UCLASS(Blueprintable)
class PACHIO_API UQuestionBlockIdleState : public UBlockState
{
	GENERATED_BODY()
public:
	virtual bool OnEnter(ABaseBlock*, UWorld*,const FString materialID = "None")override;
	virtual bool OnUpdate(ABaseBlock*)override;
	virtual bool OnExit(ABaseBlock*)override;
	virtual bool OnHit(FAttackData, const FVector = FVector(0,0,0),const AActor* = nullptr)override;
};
