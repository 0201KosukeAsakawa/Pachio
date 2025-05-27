// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Logic/KoopaShellState.h"
#include "Enemy/Logic/KoopaAliveState.h"
#include "Enemy/Logic/KoopaKickedStateState.h"
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
	pWorld = currentLevel;
	timer = 0;
	mOwner->SetHp(1.0f);
	mOwner->SetCanJamp(false);
	if (!owner->GetMesh())
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
	if (!pWorld)
		return false;

	timer += pWorld->DeltaTimeSeconds;

	if (timer < 5)
		return true;

	UKoopaAliveState* nextState = NewObject<UKoopaAliveState>(mOwner);

	logicComponent->ChangeState(nextState, mOwner);

	return true;
}

bool UKoopaShellState::OnExit()
{
	timer = 0;
	return true;
}

bool UKoopaShellState::OnOverlap(AActor* hitActor)
{
	//TODO:後で直して
	FVector direc = FVector{ 0,0,0 };

	direc.Y = mOwner->GetActorLocation().Y-hitActor->GetActorLocation().Y;

	UKoopaKickedStateState* nextState = NewObject<UKoopaKickedStateState>(mOwner);
	logicComponent->ChangeState(nextState, mOwner);
	nextState->SetDirection(direc.GetSafeNormal());
	return true;
}
