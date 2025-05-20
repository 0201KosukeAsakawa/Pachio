// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/PlayerStateComponent.h"
#include "PlayerFireState.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class PACHIO_API UPlayerFireState : public UPlayerStateComponent
{
	GENERATED_BODY()

public:
	bool OnEnter(ACharacter*, UWorld*)override;
	bool OnUpdate(float)override;
	bool OnExit(ACharacter*)override;
	bool OnSkill(const FInputActionValue&)override;
	bool TakeDamage()override;
};
