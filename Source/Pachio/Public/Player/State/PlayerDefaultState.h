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

	// アニメーション終了時のコールバック
	UFUNCTION()
	void OnLandingMontageEnded(UAnimMontage* Montage, bool bInterrupted);
	void PlayLandingAnimation();
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

	UPROPERTY(EditAnywhere, Category = "Animation")
	UAnimMontage* LandingMontage; // 着地アニメーションモンタージュ

	bool bIsPlayingLandingAnimation = false; // 着地アニメーション再生中フラグ

	FRotator InitialRotation;

	FVector CurrentDirection;

	UPROPERTY()
	UPhysicsCalculator* Physics;

	UPROPERTY()
	UCapsuleComponent* HitBox;

private:
	bool bLandingAnimationJustEnded = false; // 着地アニメーションが終了した瞬間のフラグ

public:
	// 着地アニメーション終了を確認する関数
	UFUNCTION(BlueprintCallable, Category = "Animation")
	bool HasLandingAnimationEnded() const { return bLandingAnimationJustEnded; }

	// 着地アニメーション再生中かを確認する関数
	UFUNCTION(BlueprintCallable, Category = "Animation")
	bool IsPlayingLandingAnimation() const { return bIsPlayingLandingAnimation; }
};
