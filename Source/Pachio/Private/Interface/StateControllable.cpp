// Fill out your copyright notice in the Description page of Project Settings.


#include "Interface/StateControllable.h"

// Add default functionality here for any IStateControllable functions that are not pure virtual.

bool IStateControllable::ChangeState(FString Tag)
{
	return false;
}

UPlayerStateComponent* IStateControllable::GetPlayerState() const
{
	return nullptr;
}
