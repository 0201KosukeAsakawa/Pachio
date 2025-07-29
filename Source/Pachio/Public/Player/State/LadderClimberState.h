// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/PlayerStateComponent.h"
#include "LadderClimberState.generated.h"

class ALadderActor;
class UMoveComponent;
UCLASS()
class PACHIO_API ULadderClimberState : public UPlayerStateComponent
{
	GENERATED_BODY()
public:
	void SetTargetLadder(ALadderActor*);
	void Movement(const FInputActionValue& Value)override;
private:
	// ステートに入ったときの処理
	virtual bool OnEnter(ACharacter* Owner, UWorld* World)override;

	// 毎フレームの更新処理（Tick の代わりに呼ばれる）
	virtual bool OnUpdate(float DeltaTime)override;

	// ステートから出るときの処理
	virtual bool OnExit(ACharacter* Owner)override;

	virtual bool OnSkill(const FInputActionValue& Input)override;
private:
	UPROPERTY()
	ALadderActor* Ladder;
	UPROPERTY()
	UMoveComponent* MoveComp;
};
