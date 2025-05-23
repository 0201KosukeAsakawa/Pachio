// Fill out your copyright notice in the Description page of Project Settings.


#include "Logic/Movement/PlayerMoveLogic.h"
#include "InputAction.h"
#include "GameFramework/Character.h"

// 移動処理（StateManager 経由）
FVector UPlayerMoveLogic::Movement(float DeltaTime, AActor* Owner, const FInputActionValue& Value)
{

	// 入力値（X = 左右, Y = 前後）
	FVector2D MoveInput = Value.Get<FVector2D>();

	ACharacter* Character = Cast<ACharacter>(Owner);
	if (!Character)
		return FVector(0,0,0);

	// カメラの回転から前方・右方向ベクトルを取得
	FRotator CamRot = Character->GetControlRotation();
	FVector CamForward = CamRot.Vector(); // 前方ベクトル
	FVector CamRight = FRotationMatrix(CamRot).GetUnitAxis(EAxis::Y); // 右方向ベクトル

	// ========== 実際の移動処理 ==========
	// 入力値に基づく移動方向を計算し、正規化
	return (CamRight * MoveInput.X + CamForward * MoveInput.Y).GetSafeNormal();

	/* キャラクターを移動させる
	Character->AddMovementInput(MoveDir, StateManager->GetCurrentState()->GetMoveSpeed());

	// 入力がある場合のみ、キャラクターの向きを滑らかに回転させる
	if (!MoveDir.IsNearlyZero())
	{
		// 向くべき方向を計算
		FRotator TargetRot = UKismetMathLibrary::FindLookAtRotation(
			GetActorLocation(),
			GetActorLocation() + MoveDir
		);

		// Pitch（上下）、Roll（傾き）は固定
		TargetRot.Pitch = 0.0f;
		TargetRot.Roll = 0.0f;

		// 現在の回転と目標の回転の間をスムーズに補間
		FRotator SmoothRot = FMath::RInterpTo(
			GetActorRotation(),
			TargetRot,
			GetWorld()->GetDeltaSeconds(),
			10.0f // 補間スピード
		);

		// キャラクターの回転を設定
		SetActorRotation(SmoothRot);
	}

	return;*/
}

void UPlayerMoveLogic::Init(float speed,const FVector)
{
	return;
}
