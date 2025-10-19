// Fill out your copyright notice in the Description page of Project Settings.


#include "Interface/MoveLogic.h"

// Add default functionality here for any IMoveLogic functions that are not pure virtual.

FVector IMoveLogic::Movement(float DeltaTime, AActor* Owner,const FInputActionValue& Value)
{
	return FVector(0, 0, 0);
}

void IMoveLogic::Init(const float Speed, const FVector& Direction)
{
}
