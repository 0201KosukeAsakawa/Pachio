// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/BlockState/EmptyState.h"
#include "Components/StaticMeshComponent.h"
#include "DataContainer/BlockDataContainer.h"
#include "FunctionLibrary.h"
#include "Manager/LevelManager.h"

bool UEmptyState::OnEnter(ABaseBlock* owner, UWorld* world,  FString materialID)
{
	return false;
}
