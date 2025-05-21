// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/EnemyState.h"
#include "Enemy/State/EnemyStateComponent.h"

bool UEnemyState::OnEnter(AEnemyCharacter* owner, UWorld* world, UEnemyStateComponent* LogicComponet , FString)
{
	return false;
}

bool UEnemyState::OnUpdate(float deltaTime)
{
	return false;
}

bool UEnemyState::OnExit()
{
	return false;
}

bool UEnemyState::OnOverlap(AActor*)
{
	return false;
}
