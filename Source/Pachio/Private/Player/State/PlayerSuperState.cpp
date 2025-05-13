// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/State/PlayerSuperState.h"
#include "InputActionValue.h"
#include "Interface/StateControllable.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/StaticMeshComponent.h"
#include "FunctionLibrary.h"

bool UPlayerSuperState::OnEnter(ACharacter* owner, UWorld* world)
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
		}
	}

	// 移動速度の初期値設定（ステート内で使用）
	mMoveSpeed = 100.0f;

	return true; // ステートの切り替え成功
}

bool UPlayerSuperState::OnUpdate(float)
{
	return false;
}

bool UPlayerSuperState::OnExit(ACharacter*)
{
	return false;
}

bool UPlayerSuperState::OnSkill(const FInputActionValue&)
{
	return false;
}

bool UPlayerSuperState::TakeDamage()
{
	if (!mOwner)
		return false;

	IStateControllable* is = Cast<IStateControllable>(mOwner);
	if (!is)
		return false;

	is->ChangeState("Default");
	return true;
}
