// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Logic/GoombaDeadState.h"
#include "Enemy/EnemyCharacter.h"
#include "Components/BoxComponent.h"
#include "Manager/LevelManager.h"
#include "DataContainer/EnemyDataContainer.h"

bool UGoombaDeadState::OnEnter(AEnemyCharacter* owner, UWorld* world, UEnemyStateComponent* LogicComponet, FString materialID)
{
	if (!owner || !world || !owner->GetMesh())
		return false;
	mOwner = owner;
	pWorld = world;
	deadTimer = 0.0f;
	owner->GetMesh()->SetRelativeScale3D(FVector(1.0f, 1.0f, 0.5f));

	if (!owner->GetMesh() || !owner->GetAttackCollision()) // メッシュが存在する場合
		return false;

	owner->GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision); // メッシュのコリジョンを無効にする
	owner->GetAttackCollision()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	UMaterialInterface* newMaterial = ALevelManager::GetInstance(GetWorld())->GetEnemyContainer()->CreateMaterial(world, materialID);
	if (!newMaterial)
		return false;

	owner->GetMesh()->SetMaterial(0, newMaterial);

	return true;
}

bool UGoombaDeadState::OnUpdate(float deltaTime)
{
	if (!pWorld)
		return false;

	deadTimer += pWorld->DeltaTimeSeconds;

	if (deadTimer < 3.0f)
		return true;


	// 今後の処理実装予定。現時点ではダメージ処理なし。
	mOwner->Destroy();
	return true;
}

bool UGoombaDeadState::OnExit()
{
	return false;
}

bool UGoombaDeadState::OnOverlap(AActor*)
{
	return false;
}
