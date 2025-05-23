// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Logic/KoopaKickedStateState.h"
#include "Enemy/EnemyCharacter.h"
#include "Enemy/State/EnemyStateComponent.h"
#include "Enemy/Logic/KoopaShellState.h"
#include "Components/AttackComponent.h"
#include "Components/MoveComponent.h"            // 自作の移動処理用コンポーネント
#include "Components/PhysicsCalculator.h"        // 重力などの物理演算コンポーネント
#include "Manager/LevelManager.h"
#include "DataContainer/EnemyDataContainer.h"

// コーパキャラクターが「蹴られた」状態に入る時の処理
bool UKoopaKickedStateState::OnEnter(AEnemyCharacter* owner, UWorld* world, UEnemyStateComponent* LogicComponet, const FString materialID)
{
    // オーナー、ワールド、ロジックコンポーネントが無効な場合は処理を終了
    if (!owner || !world || !LogicComponet)
        return false;

    mOwner = owner;
    logicComponent = LogicComponet;

    // ジャンプ可能に設定
    mOwner->SetCanJamp(true);

    // 移動コンポーネントの初期化
    MoveComp = NewObject<UMoveComponent>(mOwner);
    // 攻撃コンポーネントの初期化
    Attack = NewObject<UAttackComponent>(mOwner);
    mOwner->SetHp(1.0f); // HP を1に設定

    // Actor オブジェクトをキャストし、必要なコンポーネントの初期化
    AActor* actor = Cast<AActor>(mOwner);
    if (!actor || !MoveComp || !Attack || !Attack->Init(world, "DamageOnly"))
        return false;

    // 攻撃データの設定（敵キャラクターに対する攻撃、壊れない攻撃）
    Attack->SetAttackData(EAttackType::Indiscriminate, EBreakLevel::Breakable);

    // キャラクターの移動コンポーネントの初期化
    MoveComp->Init(actor, this);
    MoveComp->SetSpeed(1000.0f); // 移動速度設定

    // 物理計算コンポーネントの初期化（重力の適用など）
    PhysicsCal = NewObject<UPhysicsCalculator>(actor);

    // メッシュが無ければ処理を終了
    if (!owner->GetMesh())
        return false;

    // メッシュのコリジョンを無効にして、攻撃を受けるまで衝突しないように設定
    owner->GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    // 新しいマテリアルを作成して設定
    UMaterialInterface* newMaterial = ALevelManager::GetInstance(world)->GetEnemyContainer()->CreateMaterial(world, materialID);
    if (!newMaterial)
        return false;

    owner->GetMesh()->SetMaterial(0, newMaterial); // メッシュに新しいマテリアルを設定

    return true; // 初期化が成功した場合
}

bool UKoopaKickedStateState::OnUpdate(float DeltaTime)
{
    // 移動コンポーネントが無ければ処理を行わない
    if (!MoveComp || !mOwner || !logicComponent)
    {
        return false;
    }

    // レイキャストを行い、進行方向に障害物があるか確認
    FVector Start = mOwner->GetActorLocation(); // 現在の位置
    FVector End = Start + (mOwner->GetActorForwardVector() * 100.0f); // 進行方向に100ユニット先までレイを飛ばす（距離調整可能）

    FHitResult HitResult;
    FCollisionQueryParams CollisionParams;
    CollisionParams.AddIgnoredActor(mOwner);  // 自身を無視する

    // レイキャストを発射
    bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, CollisionParams);

    // レイキャストで何かに当たった場合
    if (bHit)
    {
        // 衝突したアクターを取得
        AActor* hitActor = HitResult.GetActor();
        // 攻撃コンポーネントが無ければ処理しない
        if (!Attack)
            return false;

        // 攻撃処理を実行
        Attack->PerformAttack(hitActor);
    }

    // 移動処理（蹴られて移動する）
    MoveComp->Movement(DeltaTime,mOwner);

    // 物理計算（重力など）を適用
    PhysicsCal->AddGravity();

    // キャラクターが死亡した場合、甲羅状態に遷移
    if (mOwner->IsDead())
    {
        UKoopaShellState* nextState = NewObject<UKoopaShellState>(mOwner);
        logicComponent->ChangeState(nextState, mOwner); // 甲羅状態に遷移
    }

    return true;
}


// 状態終了時の処理
bool UKoopaKickedStateState::OnExit()
{
    return true;
}

// 衝突時の処理
bool UKoopaKickedStateState::OnOverlap(AActor* hitActor)
{

    //// 攻撃コンポーネントが無ければ処理しない
    //if (!Attack)
    //    return false;

    //// 攻撃処理を実行
    //Attack->PerformAttack(hitActor);
    return true;
}

// 移動方向の設定
void UKoopaKickedStateState::SetDirection(FVector direction)
{
    // 移動コンポーネントに移動方向を設定ta
    MoveComp->SetDirection(direction);
}
