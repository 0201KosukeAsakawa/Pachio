// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/EnemyState.h"
#include "KoopaKickedStateState.generated.h"

/**
 * 
 */
class UMoveComponent;
class UPhysicsCalculator;
class UAttackComponent;

UCLASS()
class PACHIO_API UKoopaKickedStateState : public UEnemyState
{
	GENERATED_BODY()
	
	virtual bool OnEnter(AEnemyCharacter* owner, UWorld* world, UEnemyStateComponent* LogicComponet, const EEnemyCategory materialID)override;
	virtual bool OnUpdate(float deltaTime)override;
	virtual bool OnExit()override;
	virtual bool OnOverlap(AActor*)override;
public:
	void SetDirection(FVector);

private:
	UPROPERTY();
	UMoveComponent* MoveComp;

	UPROPERTY();
	UPhysicsCalculator* PhysicsCal;

	UPROPERTY()
	UAttackComponent* Attack;
};
