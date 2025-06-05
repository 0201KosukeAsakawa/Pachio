// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/BlockState.h"
#include "DataContainer/BlockDataContainer.h"
#include"Attack/AttackStrategy.h"
#include "Materials/MaterialInterface.h"
#include "FunctionLibrary.h"
#include "Objects/BaseBlock.h"

// Sets default values for this component's properties
UBlockState::UBlockState()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

bool UBlockState::OnEnter(ABaseBlock*, UWorld*, FString)
{
	return false;
}

bool UBlockState::OnUpdate(ABaseBlock*)
{
	return false;
}

bool UBlockState::OnExit(ABaseBlock*)
{
	return false;
}

bool UBlockState::OnHit(FAttackData, FVector,const AActor*)
{
	return false;
}
