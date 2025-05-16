// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/State/GoombaStateComponent.h"
#include "Enemy/EnemyCharacter.h"
#include "Enemy/Logic/GoombaAliveState.h"
#include "Components/EnemyState.h"
#include "Components/AttackComponent.h"
#include "Components/MoveComponent.h"            // 自作の移動処理用コンポーネント
#include "Components/PhysicsCalculator.h"        // 重力などの物理演算コンポーネント

bool UGoombaStateComponent::OnEnter(AEnemyCharacter* owner , UWorld* currentLevel, const FString materialID)
{
	if (!owner || !currentLevel)
		return false;

	logic = NewObject<UGoombaAliveState>(this);
	if (!logic)
		return false;

	logic->OnEnter(owner, currentLevel,this, materialID);

	return true;
}

bool UGoombaStateComponent::OnUpdate(float DeltaTime)
{
	if (!logic)
		return false;

	return logic->OnUpdate(DeltaTime);
}

bool UGoombaStateComponent::OnExit()
{
	if (!logic)
		return false;

	logic->OnExit();

	return true;
}

void UGoombaStateComponent::OnOverlap(AActor* hitActor)
{
	if (!logic)
		return;

	logic->OnOverlap(hitActor);
}
