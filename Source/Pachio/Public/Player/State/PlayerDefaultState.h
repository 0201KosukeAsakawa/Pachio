// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Player/PlayerStateComponent.h"
#include "PlayerDefaultState.generated.h"
class UMoveComponent;
class UBoxComponent;
UCLASS(Blueprintable)
class PACHIO_API UPlayerDefaultState : public UPlayerStateComponent
{
	GENERATED_BODY()
public:
	UPlayerDefaultState();
public:
	bool OnEnter(ACharacter*, UWorld*)override;
	bool OnUpdate(float)override;
	bool OnExit(ACharacter*)override;
	bool OnSkill(const FInputActionValue&)override;
	void Movement(const FInputActionValue& Value)override;
	void Jump(UPhysicsCalculator* physics, float jumpForce)override;

private:
	bool TryEnterLadderOnJump() const;

private:
	// プレイヤー移動処理を司るコンポーネント
	UPROPERTY()
	UMoveComponent* MoveComp;
	UPROPERTY()
	UBoxComponent* BoxComp;
	float Direction;

	float MoveSpeed = 10;

	bool InitialRotationSet;

	FRotator InitialRotation;

	FVector CurrentDirection;
};
