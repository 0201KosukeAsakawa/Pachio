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
	void Jump(float jumpForce)override;

private:

	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);

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
};
