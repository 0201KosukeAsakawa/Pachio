// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/State/GoombaStateComponent.h"
#include "Enemy/EnemyCharacter.h"
#include "Enemy/Logic/GoombaAliveState.h"
#include "Components/EnemyState.h"
#include "Components/AttackComponent.h"
#include "Components/MoveComponent.h"            // 自作の移動処理用コンポーネント
#include "Components/PhysicsCalculator.h"        // 重力などの物理演算コンポーネント

// ゴンバの状態が「Enter」した時の処理
bool UGoombaStateComponent::OnEnter(AEnemyCharacter* owner, UWorld* currentLevel, const FString materialID)
{
    // オーナーやレベルが無効な場合、処理を終了
    if (!owner || !currentLevel)
        return false;

    // ゴンバの状態が「Alive」状態として初期化
    logic = NewObject<UGoombaAliveState>(this);

    // 状態の生成に失敗した場合、処理を終了
    if (!logic)
        return false;

    // ゴンバの「Alive」状態に遷移
    logic->OnEnter(owner, currentLevel, this, materialID);

    return true; // 初期化が成功した場合
}

// 毎フレームの更新処理
bool UGoombaStateComponent::OnUpdate(float DeltaTime)
{
    // 状態が無効な場合は処理しない
    if (!logic)
        return false;

    // 現在の状態で更新処理を実行
    return logic->OnUpdate(DeltaTime);
}

// ゴンバの状態が「Exit」する時の処理
bool UGoombaStateComponent::OnExit()
{
    // 状態が無効な場合は処理しない
    if (!logic)
        return false;

    // 現在の状態を終了
    logic->OnExit();

    return true; // 終了処理が成功した場合
}

// 他のアクターとの衝突処理
void UGoombaStateComponent::OnOverlap(AActor* hitActor)
{
    // 状態が無効な場合は処理しない
    if (!logic)
        return;

    // 現在の状態で衝突処理を実行
    logic->OnOverlap(hitActor);
}
