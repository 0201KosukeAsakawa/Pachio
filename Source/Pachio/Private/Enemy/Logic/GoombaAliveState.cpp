#include "Enemy/Logic/GoombaAliveState.h"
#include "Enemy/EnemyCharacter.h"
#include "Enemy/Logic/GoombaDeadState.h"
#include "Enemy/State/EnemyStateComponent.h"
#include "Components/AttackComponent.h"
#include "Components/MoveComponent.h"            // 敵の移動を制御するコンポーネント
#include "Components/PhysicsCalculator.h"        // 重力などの物理演算を行うコンポーネント
#include "Manager/LevelManager.h"
#include "Logic/Movement/EnemyMoveLogic.h"
#include "Interface/MoveLogic.h"
#include "DataContainer/EnemyDataContainer.h"

// Goombaが生きている状態に入ったときの初期処理
bool UGoombaAliveState::OnEnter(AEnemyCharacter* owner, UWorld* currentLevel, UEnemyStateComponent* LogicComponet, EEnemyCategory materialID)
{
    // 入力引数が不正な場合は失敗として終了
    if (!owner || !currentLevel || !LogicComponet)
        return false;

    // 所有者と状態コンポーネントの参照を保存
    mOwner = owner;
    logicComponent = LogicComponet;

    // 移動コンポーネントと攻撃コンポーネントを新規作成
    MoveComp = NewObject<UMoveComponent>(mOwner);
    Attack = NewObject<UAttackComponent>(mOwner);

    // 攻撃のデータを設定（敵による攻撃で、壊れない性質）
    Attack->SetAttackData(EAttackType::Enemy, EBreakLevel::Unbreakable);

    // ジャンプ可能状態に設定（敵の移動ロジックで必要な場合）
    mOwner->SetCanJamp(true);

    // アクター型へキャスト（MoveComp の Init に使用）
    AActor* actor = Cast<AActor>(mOwner);

    // 各コンポーネントの初期化に失敗した場合は終了
    if (!actor || !MoveComp || !Attack || !Attack->Init(currentLevel, "DamageOnly"))
        return false;

    // 移動コンポーネントの初期化（初期速度100、初期向き：Yマイナス）
    MoveComp->Init(actor, NewObject<UEnemyMoveLogic>(this), 1000, FVector(-1, 0, 0));
    // MoveComp->SetSpeed(1000.0f); // 必要なら個別に速度を設定可能

    // 重力演算コンポーネントの作成
    PhysicsCal = NewObject<UPhysicsCalculator>(actor);

    // メッシュがない場合は即終了
    if (!owner->GetMesh())
        return false;

    // メッシュのコリジョンを無効化（他オブジェクトとの衝突を防ぐ）
    owner->GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    // マテリアルを作成してセット（見た目の種類設定）
    UMaterialInterface* newMaterial = ALevelManager::GetInstance(currentLevel)->GetEnemyContainer()->CreateMaterial(currentLevel, materialID);
    if (!newMaterial)
        return false;

    // 作成されたマテリアルを敵キャラに設定
    owner->GetMesh()->SetMaterial(0, newMaterial);

    return true; // 正常に初期化完了
}

// Goombaが生きている状態の更新処理（毎フレーム呼ばれる）
bool UGoombaAliveState::OnUpdate(float DeltaTime)
{
    // 主要なコンポーネントが無効なら処理中断
    if (!MoveComp || !mOwner || !logicComponent)
        return false;

    // 移動位置の計算と反映（現在位置から目標位置へ）
    FVector v = MoveComp->Movement(DeltaTime, mOwner , MoveDirection);
    FVector m = v - mOwner->GetActorLocation();
    mOwner->SetActorLocation(v);

    // 死亡状態なら死亡ステートへ遷移
    if (mOwner->IsDead())
    {
        UGoombaDeadState* nextState = NewObject<UGoombaDeadState>(mOwner);
        logicComponent->ChangeState(nextState, mOwner);
    }

    return true; // 更新成功
}

// Goombaがこのステートを抜けるときの処理
bool UGoombaAliveState::OnExit()
{
    // 今のところ特別な処理なし
    return true;
}

// 敵が他オブジェクトと接触した際の処理
bool UGoombaAliveState::OnOverlap(AActor* hitActor)
{
    // 攻撃コンポーネントがない場合は処理不能
    if (!Attack)
        return false;

    // 接触対象へ攻撃を実行
    Attack->PerformAttack(hitActor);
    return true;
}
