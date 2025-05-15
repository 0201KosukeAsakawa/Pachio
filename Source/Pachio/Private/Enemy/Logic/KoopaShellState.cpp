// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Logic/KoopaShellState.h"
#include "Enemy/EnemyCharacter.h"
#include "Enemy/State/EnemyStateComponent.h"
#include "Components/AttackComponent.h"
#include "Components/MoveComponent.h"            // 自作の移動処理用コンポーネント
#include "Components/PhysicsCalculator.h"        // 重力などの物理演算コンポーネント
#include "Manager/LevelManager.h"
#include "DataContainer/EnemyDataContainer.h"

bool UKoopaShellState::OnEnter(AEnemyCharacter* owner, UWorld* currentLevel, UEnemyStateComponent* LogicComponet, const FString materialID)
{
	if (!owner || !currentLevel || !LogicComponet)
		return false;

	mOwner = owner;
	logicComponent = LogicComponet;

	AActor* actor = Cast<AActor>(mOwner);
	if (!actor)
		return false;


	if (!owner->GetMesh()) // メッシュが存在しない場合
		return false;

	owner->GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision); // メッシュのコリジョンを無効にする

	UMaterialInterface* newMaterial = ALevelManager::GetInstance(currentLevel)->GetEnemyContainer()->CreateMaterial(currentLevel, /*materialID*/"Koopa" , "Shell");
	if (!newMaterial)
		return false;

	owner->GetMesh()->SetMaterial(0, newMaterial);

	return true;
}

bool UKoopaShellState::OnUpdate(float deltaTime)
{
	return true;
}

bool UKoopaShellState::OnExit()
{
	return true;
}

bool UKoopaShellState::OnOverlap(AActor* hitActor)
{
	//TODO:後で直して
	return true;
}
