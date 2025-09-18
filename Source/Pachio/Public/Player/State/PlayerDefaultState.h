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
	bool OnEnter(APawn*, UWorld*)override;
	bool OnUpdate(float)override;
	bool OnExit(APawn*)override;
	bool OnSkill(const FInputActionValue&)override;
	void Movement(const FInputActionValue& Value)override;
	bool Jump(float jumpForce)override;

private:
	bool TryEnterLadderOnJump() const;

	void UpdateInteractableUI();
	void CheckHoldableObject();
	void CheckLadderObject();

	bool CheckObjectByTag(const FVector& Start, const FVector& End, const FCollisionShape& Shape, const FName& Tag, bool& bPrevState, const FName& WidgetName, const FName& AnimName);

private:
	FTimerHandle CheckHoldableHandle;
	// プレイヤー移動処理を司るコンポーネント
	UPROPERTY()
	UMoveComponent* MoveComp;
	UPROPERTY()
	UBoxComponent* BoxComp;
	float Direction;

	float MoveSpeed = 10;

	bool InitialRotationSet;

	bool bPrevCanHold;
	bool bPrevCanClim;

	FRotator InitialRotation;

	FVector CurrentDirection;

	UPROPERTY()
	UPhysicsCalculator* Physics;

	UPROPERTY()
	UBoxComponent* BoxComponent;
};
