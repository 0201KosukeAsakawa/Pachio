// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/State/BlueStateManager.h"

// Sets default values for this component's properties
UBlueStateManager::UBlueStateManager()
{

}

void UBlueStateManager::Init_Implementation(APawn* Owner, UWorld* World)
{
	UStateManagerBase::Init_Implementation(Owner, World);
}

void UBlueStateManager::Update_Implementation(float DeltaTime)
{
}