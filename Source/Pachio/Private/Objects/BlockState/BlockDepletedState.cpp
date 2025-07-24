// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/BlockState/BlockDepletedState.h"
#include "Objects/BaseBlock.h"
#include "Attack/AttackStrategy.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInterface.h"
#include "Manager/LevelManager.h"
#include "FunctionLibrary.h"

#include "DataContainer/BlockDataContainer.h"

bool UBlockDepletedState::OnEnter(ABaseBlock* owner, UWorld* world, FString materialID)
{
    return true;
}

bool UBlockDepletedState::OnUpdate(ABaseBlock*)
{
    return true;
}
bool UBlockDepletedState::OnExit(ABaseBlock*)
{
    return true;
}
bool UBlockDepletedState::OnHit(FAttackData , FVector , const AActor*)
{
    mOwner->Destroy();
    return true;
}
