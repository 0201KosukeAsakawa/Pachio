// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Player/PlayerStateComponent.h"
#include "PlayerDefaultState.generated.h"
class UMoveComponent;
UCLASS(Blueprintable)
class PACHIO_API UPlayerDefaultState : public UPlayerStateComponent
{
	GENERATED_BODY()

public:
	bool OnEnter(ACharacter*, UWorld*)override;
	bool OnUpdate(float)override;
	bool OnExit(ACharacter*)override;
	bool OnSkill(const FInputActionValue&)override;
	void Movement(const FInputActionValue& Value)override;

private:
	// プレイヤー移動処理を司るコンポーネント
	UPROPERTY()
	UMoveComponent* MoveComp;
	float MoveSpeed = 10;
};
