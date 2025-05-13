// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/EnemyState.h"
#include "GoombaAliveState.generated.h"

class UMoveComponent;
class UPhysicsCalculator;
class UAttackComponent;

UCLASS()
class PACHIO_API UGoombaAliveState : public UEnemyState
{
	GENERATED_BODY()
private:
	virtual bool OnEnter(AEnemyCharacter* owner , UWorld* world)override;
	virtual bool OnUpdate(float deltaTime)override;
	virtual bool OnExit()override;

private:
	UPROPERTY();
	UMoveComponent* MoveComp;

	UPROPERTY();
	UPhysicsCalculator* PhysicsCal;

	UPROPERTY()
	UAttackComponent* Attack;
};
