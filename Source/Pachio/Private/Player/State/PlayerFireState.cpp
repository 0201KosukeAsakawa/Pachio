// Fill out your copyright notice in the Description page of Project Settings.

#include "Player/State/PlayerFireState.h"
#include "Components/StaticMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "FunctionLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "InputActionValue.h"
#include "Interface/StateControllable.h"
#include "Player/PlayerCharacter.h"

// ステートに入る際に実行される処理
bool UPlayerFireState::OnEnter(ACharacter* owner, UWorld* world)
{
	if (owner == nullptr || world == nullptr)
	{
		return false;
	}

	// 内部に所有者とワールドを保存
	mOwner = owner;
	pWorld = world;

	// マテリアルの設定（デフォルトステート用）
	//if (NewMaterial)
	{
		// キャラクターが持つ StaticMeshComponent を取得
		UStaticMeshComponent* StaticMeshComp = UFunctionLibrary::FindComponentByName<UStaticMeshComponent>(owner, "StaticMesh");
		UMaterialInterface* N = NewMaterial.LoadSynchronous(); // 非同期ロードに対応
		if (N != nullptr && StaticMeshComp)
		{
			StaticMeshComp->SetMaterial(0, N); // マテリアルをスロット0に適用

			//マテリアルのサイズを拡大
			StaticMeshComp->SetRelativeScale3D(FVector(1.0, 1.0, 2.0));
		}
	}

	//コリジョンサイズ変更
	mOwner->GetCharacterMovement()->Crouch();
	mOwner->GetCapsuleComponent()->SetCapsuleHalfHeight(110.0);

	APlayerCharacter* aPlayer = Cast<APlayerCharacter>(mOwner);
	if (!aPlayer)
		return false;

	//コリジョン位置調整
	aPlayer->PowerUpCollisionPosition();

	// 移動速度の初期値設定（ステート内で使用）
	mMoveSpeed = 100.0f;

	return true; // ステートの切り替え成功
}

// ステートの毎フレーム更新処理（現時点では何もしない）
bool UPlayerFireState::OnUpdate(float)
{
	return false;
}

// ステートを離脱するときの処理（現時点では何もしない）
bool UPlayerFireState::OnExit(ACharacter*)
{
	return false;
}

// スキルボタン入力時の処理（現時点では何もしない）
bool UPlayerFireState::OnSkill(const FInputActionValue&)
{
	return false;
}

//ダメージを受けたときの処理
bool UPlayerFireState::TakeDamage()
{
	if (!mOwner)
		return false;

	IStateControllable* is = Cast<IStateControllable>(mOwner);
	if (!is)
		return false;

	is->ChangeState("Default");
	return true;
}
