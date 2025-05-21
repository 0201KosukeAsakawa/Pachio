// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interface/MoveLogic.h"

/**
 * 
 */
class PACHIO_API EnemyMoveLogic :public IMoveLogic
{
public:
	EnemyMoveLogic();
	~EnemyMoveLogic();

private:
	FVector Movement(float DeltaTime, AActor* Owner, const FInputActionValue& Value = FInputActionValue())override;
	bool IsCollidingWithWall(FVector Direction, const AActor* Owner);

private:
	float Speed;
	FVector CurrentMovementDirection;
};
