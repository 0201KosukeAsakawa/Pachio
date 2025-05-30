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
#include "Logic/Movement/EnemyMoveLogic.h"

// コーパキャラクターが「生存」状態に入る時の処理
bool UKoopaAliveState::OnEnter(AEnemyCharacter* owner, UWorld* currentLevel, UEnemyStateComponent* LogicComponet, const EEnemyCategory materialID)
{
    // オーナー、レベル、ロジックコンポーネントが無効な場合は処理を終了
    if (!owner || !currentLevel || !LogicComponet)
        return false;

    mOwner = owner;
    logicComponent = LogicComponet;

    // ジャンプ可能に設定
    mOwner->SetCanJamp(true);

    // 移動コンポーネントを初期化
    MoveComp = NewObject<UMoveComponent>(mOwner);
    // 攻撃コンポーネントの初期化
    Attack = NewObject<UAttackComponent>(mOwner);
   
    // 物理計算用のコンポーネントを初期化
    AActor* actor = Cast<AActor>(mOwner);
    if (!actor || !MoveComp || !Attack || !Attack->Init(currentLevel, "DamageOnly"))
        return false;
    Attack->SetAttackData(EAttackType::Enemy, EBreakLevel::Unbreakable);
    // キャラクターの移動コンポーネント初期化
    MoveComp->Init(actor, NewObject<UEnemyMoveLogic>(this),10.0f,FVector(0,-1,0));
    //MoveComp->SetSpeed(10.0f); // 移動速度設定

    // 物理計算コンポーネントを初期化（重力などの適用）
    PhysicsCal = NewObject<UPhysicsCalculator>(actor);

    // メッシュが無ければ処理を終了
    if (!owner->GetMesh())
        return false;

    // メッシュのコリジョンを無効にする（攻撃を受けるまで衝突しないようにする）
    owner->GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    // 新しいマテリアルを作成して設定
    UMaterialInterface* newMaterial = ALevelManager::GetInstance(currentLevel)->GetEnemyContainer()->CreateMaterial(currentLevel, EEnemyCategory::Koopa /*"Koopa"*/ , "Default");
	if (!newMaterial)
		return false;

    owner->GetMesh()->SetMaterial(0, newMaterial); // メッシュに新しいマテリアルを設定

    // 初期HPを設定
    mOwner->SetHp(1.0f);

    return true; // 初期化が成功した場合
}

// 生存状態で毎フレーム実行される処理
bool UKoopaAliveState::OnUpdate(float DeltaTime)
{
    // 移動コンポーネントが無ければ処理を行わない
    if (!MoveComp || !mOwner || !logicComponent)
    {
        return false;
    }

    // 移動処理（追跡やパトロールなど）
    FVector v = MoveComp->Movement(DeltaTime, mOwner);
    FVector m = v - mOwner->GetActorLocation();
    mOwner->SetActorLocation(v);

    // 物理演算（重力など）を適用
    PhysicsCal->AddGravity();

    // キャラクターが死亡した場合、次の状態（甲羅状態）に遷移
    if (mOwner->IsDead())
    {
        UKoopaShellState* nextState = NewObject<UKoopaShellState>(mOwner);
        logicComponent->ChangeState(nextState, mOwner); // 甲羅状態に遷移
    }

    return true;
}

// 状態終了時の処理（特に必要な処理はなし）
bool UKoopaAliveState::OnExit()
{
    return true;
}

// 衝突時の処理
bool UKoopaAliveState::OnOverlap(AActor* hitActor)
{
    // 攻撃コンポーネントが無ければ処理しない
    if (!Attack)
        return false;

    // 攻撃処理を実行
    Attack->PerformAttack(hitActor);
    return true;
}
