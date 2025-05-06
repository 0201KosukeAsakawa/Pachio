// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/State/GoombaStateComponent.h"
#include "Enemy/EnemyCharacter.h"
#include "Components/MoveComponent.h"            // 自作の移動処理用コンポーネント
#include "Components/PhysicsCalculator.h"        // 重力などの物理演算コンポーネント

bool UGoombaStateComponent::OnEnter(AEnemyCharacter* owner)
{
	if (!owner)
		return false;

	mOwner = owner;

	// 移動コンポーネントのインスタンスを生成して初期化
	MoveComp = NewObject<UMoveComponent>();
	AActor* actor = Cast<AActor>(mOwner);
	if (!actor)
		return false;

	if (MoveComp)
	{
		MoveComp->Init(actor);  // キャラクター自身を渡して初期化
		MoveComp->SetSpeed(0.0f);
	}

	// 物理計算コンポーネントの生成（重力など）
	PhysicsCal = NewObject<UPhysicsCalculator>(actor);

	return true;
}

bool UGoombaStateComponent::OnUpdate(float DeltaTime)
{

	// 移動コンポーネントが無ければ処理しない
	if (!MoveComp || !mOwner)
	{
		return false;
	}

	// 移動処理（例：追跡、パトロールなど）
	MoveComp->Movement(DeltaTime);

	// 重力の適用（または物理的な補正処理）
	PhysicsCal->AddGravity();


	if (mOwner->IsDead())
	{
		// 今後の処理実装予定。現時点ではダメージ処理なし。
		mOwner->Destroy();
	}

	return true;
}

bool UGoombaStateComponent::OnExit()
{
	return true;
}
