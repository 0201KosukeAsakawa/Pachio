// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/EnemyState.h"
#include "GoombaDeadState.generated.h"

/**
 * 
 */
UCLASS()
class PACHIO_API UGoombaDeadState : public UEnemyState
{
	GENERATED_BODY()
public:
	virtual bool OnEnter(AEnemyCharacter* owner, UWorld* world, UEnemyStateComponent* LogicComponet,const FString materialID = " ")override;
	virtual bool OnUpdate(float deltaTime)override;
	virtual bool OnExit()override;
	virtual bool OnOverlap(AActor*)override;


private:
	float deadTimer;
	UPROPERTY()
	UWorld* pWorld;
};
