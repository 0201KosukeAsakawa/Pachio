// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/PlayerStateBase.h"
#include "InputActionValue.h"

// Sets default values for this component's properties
UPlayerStateBase::UPlayerStateBase()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

bool UPlayerStateBase::OnEnter(ACharacter*, UWorld*)
{
	return false;
}

bool UPlayerStateBase::OnUpdate(ACharacter*)
{
	return false;
}

bool UPlayerStateBase::OnExit(ACharacter*)
{
	return false;
}

bool UPlayerStateBase::OnSkill(const FInputActionValue&)
{
	return false;
}

void UPlayerStateBase::Jump(const FInputActionValue& Value)
{
}

void UPlayerStateBase::Movement(const FInputActionValue& Value)
{
}

