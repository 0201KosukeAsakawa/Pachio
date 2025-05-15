// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/State/KoopaComponent.h"
#include "Enemy/Logic/KoopaAliveState.h"

bool UKoopaComponent::OnEnter(AEnemyCharacter* owner, UWorld* currentLevel, const FString materialID)
{
	if(!owner || !currentLevel)
		return false;

	logic = NewObject<UKoopaAliveState>(this);
	if (!logic)
		return false;

	logic->OnEnter(owner, currentLevel, this, materialID);

	return true;
}

bool UKoopaComponent::OnUpdate(float DeltaTime)
{
	if (!logic)
		return false;

	return logic->OnUpdate(DeltaTime);
}

bool UKoopaComponent::OnExit()
{
	return true;
}

void UKoopaComponent::OnOverlap(AActor* hitActor)
{
	if (!logic)
		return;

	logic->OnOverlap(hitActor);
}
