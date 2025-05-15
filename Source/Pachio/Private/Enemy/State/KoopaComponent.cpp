// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/State/KoopaComponent.h"


bool UKoopaComponent::OnEnter(AEnemyCharacter* Owner, UWorld*, const FString materialID)
{
	return false;
}

bool UKoopaComponent::OnUpdate(float DeltaTime)
{
	return false;
}

bool UKoopaComponent::OnExit()
{
	return false;
}

void UKoopaComponent::OnOverlap(AActor*)
{
}
