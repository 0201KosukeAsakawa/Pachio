// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/BlockState.h"
#include"Attack/AttackStrategy.h"

// Sets default values for this component's properties
UBlockState::UBlockState()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

bool UBlockState::OnEnter(AActor*, UWorld*)
{
	return false;
}

bool UBlockState::OnUpdate(AActor*)
{
	return false;
}

bool UBlockState::OnExit(AActor*)
{
	return false;
}

bool UBlockState::OnHit(FVector, FAttackData)
{
	return false;
}
