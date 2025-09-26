// Fill out your copyright notice in the Description page of Project Settings.


#include "Interface/StateControllable.h"

// Add default functionality here for any IStateControllable functions that are not pure virtual.

UPlayerStateComponent* IStateControllable::ChangeState(EPlayerStateType Tag)
{
	return nullptr;
}

UPlayerStateComponent* IStateControllable::GetPlayerState() const
{
	return nullptr;
}
