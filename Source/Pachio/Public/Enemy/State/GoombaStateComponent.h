// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/State/EnemyStateComponent.h"
#include "GoombaStateComponent.generated.h"

class UMoveComponent;
class UPhysicsCalculator;
class UAttackComponent;
class UEnemyState;

UCLASS()
class PACHIO_API UGoombaStateComponent : public UEnemyStateComponent
{
	GENERATED_BODY()


public:
	// 敵キャラクターがこの状態に入ったときに呼ばれます。
    // ここでは、状態に応じた初期化（ビジュアルの変更など）を行います。
	virtual bool OnEnter(AEnemyCharacter* Owner, UWorld*, const EEnemyCategory materialID)override;

	// 状態ごとの挙動を毎フレーム更新します。
	// この関数で、移動や攻撃など、状態に基づくロジックを実行します。
	virtual bool OnUpdate(float DeltaTime)override;

	// 敵キャラクターが状態を終了する際に呼ばれます。
	// ここでは、状態から抜ける際に必要なリセットやクリーンアップを行います。
	virtual bool OnExit()override;

	void OnOverlap(AActor*)override;

private:
	UPROPERTY();
	UMoveComponent* MoveComp;

	UPROPERTY();
	UPhysicsCalculator* PhysicsCal;

	UPROPERTY()
	UAttackComponent* Attack;
};
