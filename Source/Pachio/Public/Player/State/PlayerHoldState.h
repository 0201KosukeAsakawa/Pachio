// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Player/PlayerStateComponent.h"
#include "PlayerHoldState.generated.h"

class UMoveComponent;
UCLASS()
class PACHIO_API UPlayerHoldState : public UPlayerStateComponent
{
	GENERATED_BODY()
public:
	UPlayerHoldState();	
	void SetUp(AActor*);
private:
	bool OnEnter(ACharacter* owner, UWorld* world)override;

	// 毎フレームの更新処理（Tick の代わりに呼ばれる）
	bool OnUpdate(float DeltaTime)override;
	bool OnExit(ACharacter* owner)override;

	bool OnSkill(const FInputActionValue& Value)override;

	void Movement(const FInputActionValue& Value)override;


private:
	UPROPERTY()
	AActor* HoldTarget;
	UPROPERTY()
	UMoveComponent* MoveComp;
	float InitialHoldDistance;
};
