// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interface/MoveLogic.h"
#include "StraightMovement.generated.h"


/**
 * 
 */
UCLASS()
class PACHIO_API UStraightMovement :public UObject,public IMoveLogic
{

	GENERATED_BODY()
public:
	UStraightMovement();

private:
	FVector Movement(float DeltaTime, AActor* Owner,const FInputActionValue& Value = FInputActionValue())override;
	bool IsCollidingWithWall(FVector Direction, const AActor* Owner);
	void Init(const float Speed, const FVector& Direction)override;

private:
	float Speed;
	FVector CurrentMovementDirection;
};
