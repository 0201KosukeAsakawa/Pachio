// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interface/MoveLogic.h"
#include "EnemyMoveLogic.generated.h"


/**
 * 
 */
UCLASS()
class PACHIO_API UEnemyMoveLogic :public UObject,public IMoveLogic
{

	GENERATED_BODY()
public:
	UEnemyMoveLogic();

private:
	FVector Movement(float DeltaTime, AActor* Owner, const FInputActionValue& Value = FInputActionValue())override;
	bool IsCollidingWithWall(FVector Direction, const AActor* Owner);
	void Init(float speed, const FVector = FVector(0,0,0))override;

private:
	float Speed;
	FVector CurrentMovementDirection;
};
