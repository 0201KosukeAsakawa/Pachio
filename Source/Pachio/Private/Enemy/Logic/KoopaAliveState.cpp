// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Logic/KoopaAliveState.h"
#include "Enemy/Logic/KoopaShellState.h"
#include "Enemy/EnemyCharacter.h"
#include "Enemy/State/EnemyStateComponent.h"
#include "Components/AttackComponent.h"
#include "Components/MoveComponent.h"            // 自作の移動処理用コンポーネント
#include "Components/PhysicsCalculator.h"        // 重力などの物理演算コンポーネント
#include "Manager/LevelManager.h"
#include "DataContainer/EnemyDataContainer.h"

bool UKoopaAliveState::OnEnter(AEnemyCharacter* owner, UWorld* currentLevel, UEnemyStateComponent* LogicComponet, const FString materialID)
{
	if (!owner || !currentLevel || !LogicComponet)
		return false;

	mOwner = owner;
	logicComponent = LogicComponet;

	// 移動コンポーネントのインスタンスを生成して初期化
	MoveComp = NewObject<UMoveComponent>(mOwner);
	Attack = NewObject<UAttackComponent>(mOwner);

	AActor* actor = Cast<AActor>(mOwner);
	if (!actor || !MoveComp || !Attack || !Attack->Init(currentLevel, "DamageOnly"))
		return false;
	MoveComp->Init(actor);  // キャラクター自身を渡して初期化
	MoveComp->SetSpeed(10.0f);

	// 物理計算コンポーネントの生成（重力など）
	PhysicsCal = NewObject<UPhysicsCalculator>(actor);

	if (!owner->GetMesh()) // メッシュが存在しない場合
		return false;

	owner->GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision); // メッシュのコリジョンを無効にする

	UMaterialInterface* newMaterial = ALevelManager::GetInstance(currentLevel)->GetEnemyContainer()->CreateMaterial(currentLevel, materialID);
	if (!newMaterial)
		return false;

	owner->GetMesh()->SetMaterial(0, newMaterial);

	return true;
}

bool UKoopaAliveState::OnUpdate(float DeltaTime)
{
	// 移動コンポーネントが無ければ処理しない
	if (!MoveComp || !mOwner || !logicComponent)
	{
		return false;
	}

	// 移動処理（例：追跡、パトロールなど）
	MoveComp->Movement(DeltaTime);

	// 重力の適用（または物理的な補正処理）
	PhysicsCal->AddGravity();


	if (mOwner->IsDead())
	{
		UKoopaShellState* nextState = NewObject<UKoopaShellState>(mOwner);

		logicComponent->ChangeState(nextState, mOwner);
	}

	return true;
}

bool UKoopaAliveState::OnExit()
{
	return true;
}

bool UKoopaAliveState::OnOverlap(AActor* hitActor)
{
	if (!Attack)
		return false;

	Attack->PerformAttack(hitActor);
	return true;
}
