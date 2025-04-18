// Fill out your copyright notice in the Description page of Project Settings.


#include "Interface/StateBase.h"

// Add default functionality here for any IStateBase functions that are not pure virtual.

bool IStateBase::OnEnter(AActor*, UWorld*)
{
	return false;
}

bool IStateBase::OnUpdate(AActor*, UWorld*)
{
	return false;
}

bool IStateBase::OnExit(AActor*, UWorld*)
{
	return false;
}
