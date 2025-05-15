// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Logic/KoopaKickedStateState.h"
#include "Enemy/EnemyCharacter.h"
#include "Enemy/State/EnemyStateComponent.h"
#include "Components/AttackComponent.h"
#include "Components/MoveComponent.h"            // 自作の移動処理用コンポーネント
#include "Components/PhysicsCalculator.h"        // 重力などの物理演算コンポーネント
#include "Manager/LevelManager.h"
#include "DataContainer/EnemyDataContainer.h"

bool UKoopaKickedStateState::OnEnter(AEnemyCharacter* owner, UWorld* world, UEnemyStateComponent* LogicComponet, const FString materialID)
{
	if (!owner || !world || !LogicComponet)
		return false;

	mOwner = owner;
	logicComponent = LogicComponet;

	// 移動コンポーネントのインスタンスを生成して初期化
	MoveComp = NewObject<UMoveComponent>(mOwner);
	Attack = NewObject<UAttackComponent>(mOwner);

	AActor* actor = Cast<AActor>(mOwner);
	if (!actor || !MoveComp || !Attack || !Attack->Init(world, "DamageOnly"))
		return false;
	MoveComp->Init(actor);  // キャラクター自身を渡して初期化
	MoveComp->SetSpeed(10.0f);

	// 物理計算コンポーネントの生成（重力など）
	PhysicsCal = NewObject<UPhysicsCalculator>(actor);

	if (!owner->GetMesh()) // メッシュが存在しない場合
		return false;

	owner->GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision); // メッシュのコリジョンを無効にする

	UMaterialInterface* newMaterial = ALevelManager::GetInstance(world)->GetEnemyContainer()->CreateMaterial(world, materialID);
	if (!newMaterial)
		return false;

	owner->GetMesh()->SetMaterial(0, newMaterial);

	return true;
}

bool UKoopaKickedStateState::OnUpdate(float deltaTime)
{
	return false;
}

bool UKoopaKickedStateState::OnExit()
{
	return false;
}

bool UKoopaKickedStateState::OnOverlap(AActor*)
{
	return false;
}
