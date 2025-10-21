// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/State/BlueStateManager.h"

// Sets default values for this component's properties
UBlueStateManager::UBlueStateManager()
{

}

void UBlueStateManager::Init_Implementation(APawn* Owner, UWorld* World)
{
}

void UBlueStateManager::Update_Implementation(float DeltaTime)
{
}

UPlayerStateComponent* UBlueStateManager::ChangeState_Implementation(EPlayerStateType NextStateTag)
{
	return nullptr;
}

bool UBlueStateManager::IsStateMatch_Implementation(EPlayerStateType StateTag)
{
	return false;
}
