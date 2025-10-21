// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/State/StateManagerBase.h"

// Sets default values for this component's properties
UStateManagerBase::UStateManagerBase() :
										CurrentState(nullptr)
										,mOwner(nullptr)
{

}

void UStateManagerBase::Init_Implementation(APawn* Owner, UWorld* World)
{
}

void UStateManagerBase::Update_Implementation(float DeltaTime)
{
}

UPlayerStateComponent* UStateManagerBase::ChangeState_Implementation(EPlayerStateType NextStateTag)
{
	return nullptr;
}

bool UStateManagerBase::IsStateMatch_Implementation(EPlayerStateType StateTag)
{
	return false;
}

