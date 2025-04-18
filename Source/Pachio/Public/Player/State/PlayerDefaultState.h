// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/PlayerStateBase.h"
#include "PlayerDefaultState.generated.h"

/**
 * 
 */
UCLASS()
class PACHIO_API UPlayerDefaultState : public UPlayerStateBase
{
	GENERATED_BODY()

public:
	bool OnEnter(ACharacter*, UWorld*)override;
	bool OnUpdate(ACharacter*)override;
	bool OnExit(ACharacter*)override;
	bool OnSkill(const FInputActionValue&)override;
	void Jump(const FInputActionValue& Value)override;
	void Movement(const FInputActionValue& Value)override;
};
