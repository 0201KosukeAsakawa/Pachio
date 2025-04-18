// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/State/PlayerDefaultState.h"
#include "InputActionValue.h"
#include "Kismet/KismetMathLibrary.h"

bool UPlayerDefaultState::OnEnter(ACharacter*owner, UWorld*world)
{
	if (owner == nullptr || world == nullptr)
	{
		return false;
	}
	mOwner = owner;
	pWorld = world;

	mMoveSpeed = 100.0f;


	return true;
}

bool UPlayerDefaultState::OnUpdate(ACharacter*)
{
	return true;
}

bool UPlayerDefaultState::OnExit(ACharacter*)
{
	return true;
}

bool UPlayerDefaultState::OnSkill(const FInputActionValue&)
{
	return true;
}

void UPlayerDefaultState::Jump(const FInputActionValue& Value)
{
	return;
}

//各Stateの移動処理
void UPlayerDefaultState::Movement(const FInputActionValue& Value)
{
	if (!mOwner || !pWorld)
		return;
	FVector2D MoveInput = Value.Get<FVector2D>();
	FRotator CamRot = mOwner->GetControlRotation();
	FVector CamForward = CamRot.Vector();
	FVector CamRight = FRotationMatrix(CamRot).GetUnitAxis(EAxis::Y);

	// ===========================
	// 通常移動
	// ===========================

		// 移動処理
	FVector MoveDir = (CamRight * MoveInput.X + CamForward * MoveInput.Y).GetSafeNormal();
	mOwner->AddMovementInput(MoveDir, mMoveSpeed);

	// 回転処理（前方向に向く）
	if (!MoveDir.IsNearlyZero())
	{
		FRotator TargetRot = UKismetMathLibrary::FindLookAtRotation(mOwner->GetActorLocation(), mOwner->GetActorLocation() + MoveDir);
		TargetRot.Pitch = 0.0f;
		TargetRot.Roll = 0.0f;
		FRotator SmoothRot = FMath::RInterpTo(mOwner->GetActorRotation(), TargetRot, pWorld->GetDeltaSeconds(), 10.0f);
		mOwner->SetActorRotation(SmoothRot);
	}
	return;
}