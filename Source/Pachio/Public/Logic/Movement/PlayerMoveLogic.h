// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputAction.h"
#include "Interface/MoveLogic.h"
#include "UObject/NoExportTypes.h"
#include "PlayerMoveLogic.generated.h"

/**
 * 
 */
UCLASS()
class PACHIO_API UPlayerMoveLogic :public UObject, public IMoveLogic
{
	GENERATED_BODY()

private:
	FVector Movement(float DeltaTime, AActor* Owner,const FInputActionValue& Value = FInputActionValue())override;

	void Init(const float Speed, const FVector& Direction)override;
};
