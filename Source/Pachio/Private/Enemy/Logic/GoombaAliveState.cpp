// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Logic/GoombaAliveState.h"
#include "Enemy/EnemyCharacter.h"
#include "Enemy/Logic/GoombaDeadState.h"
#include "Enemy/State/EnemyStateComponent.h"
#include "Components/AttackComponent.h"
#include "Components/MoveComponent.h"            // 自作の移動処理用コンポーネント
#include "Components/PhysicsCalculator.h"        // 重力などの物理演算コンポーネント
#include "Manager/LevelManager.h"
#include "DataContainer/EnemyDataContainer.h"

// ゴンバの生存状態に入る時の処理
bool UGoombaAliveState::OnEnter(AEnemyCharacter* owner, UWorld* currentLevel, UEnemyStateComponent* LogicComponet, FString materialID)
{
    // 必要なポインタが無効であれば処理を終了
    if (!owner || !currentLevel || !LogicComponet)
        return false;

    // オーナーとロジックコンポーネントの設定
    mOwner = owner;
    logicComponent = LogicComponet;

    // 移動コンポーネントのインスタンスを生成して初期化
    MoveComp = NewObject<UMoveComponent>(mOwner);
    Attack = NewObject<UAttackComponent>(mOwner);

    // 攻撃データの設定（敵キャラクターに対する攻撃、壊れない攻撃）
    Attack->SetAttackData(EAttackType::Enemy, EBreakLevel::Unbreakable);

    // ジャンプ可能に設定
    mOwner->SetCanJamp(true);

    // キャラクターのアクターとして設定
    AActor* actor = Cast<AActor>(mOwner);

    // アクター、移動コンポーネント、攻撃コンポーネント、攻撃の初期化が失敗した場合は終了
    if (!actor || !MoveComp || !Attack || !Attack->Init(currentLevel, "DamageOnly"))
        return false;

    // 移動コンポーネントを初期化
    MoveComp->Init(actor);
    MoveComp->SetSpeed(1000.0f); // 移動速度を設定

    // 物理計算コンポーネント（重力など）を生成
    PhysicsCal = NewObject<UPhysicsCalculator>(actor);

    // メッシュが存在しない場合は終了
    if (!owner->GetMesh())
        return false;

    // メッシュのコリジョンを無効にする（物理的な衝突を無効化）
    owner->GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    // マテリアルの作成
    UMaterialInterface* newMaterial = ALevelManager::GetInstance(currentLevel)->GetEnemyContainer()->CreateMaterial(currentLevel, materialID);

    // 新しいマテリアルの生成が失敗した場合は終了
    if (!newMaterial)
        return false;

    // メッシュに新しいマテリアルを適用
    owner->GetMesh()->SetMaterial(0, newMaterial);

    return true; // 初期化が成功した場合
}

// ゴンバの生存状態で毎フレーム実行される処理
bool UGoombaAliveState::OnUpdate(float DeltaTime)
{
    // 移動コンポーネントやオーナー、ロジックが無効であれば処理しない
    if (!MoveComp || !mOwner || !logicComponent)
    {
        return false;
    }

    // 移動処理（追跡やパトロールなど）
    MoveComp->Movement(DeltaTime);

    // 重力を適用する
    PhysicsCal->AddGravity();

    // オーナーが死亡している場合は、死亡状態に遷移
    if (mOwner->IsDead())
    {
        UGoombaDeadState* nextState = NewObject<UGoombaDeadState>(mOwner);
        logicComponent->ChangeState(nextState, mOwner);
    }

    return true; // 更新が成功した場合
}

// ゴンバの生存状態を終了する際の処理
bool UGoombaAliveState::OnExit()
{
    // 何も特別な処理は行わない（終了処理が必要なら追加）
    return true;
}

// 衝突したオブジェクトとの処理（攻撃判定）
bool UGoombaAliveState::OnOverlap(AActor* hitActor)
{
    // 攻撃コンポーネントが無ければ処理しない
    if (!Attack)
        return false;

    // 攻撃を実行する
    Attack->PerformAttack(hitActor);
    return true;
}
