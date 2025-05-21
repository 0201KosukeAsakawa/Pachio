// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Logic/GoombaDeadState.h"
#include "Enemy/EnemyCharacter.h"
#include "Components/BoxComponent.h"
#include "Manager/LevelManager.h"
#include "DataContainer/EnemyDataContainer.h"

// ゴンバが死亡状態に入る時の処理
bool UGoombaDeadState::OnEnter(AEnemyCharacter* owner, UWorld* world, UEnemyStateComponent* LogicComponet, FString materialID)
{
    // 必要なポインタが無効であれば処理を終了
    if (!owner || !world || !owner->GetMesh())
        return false;

    // オーナー、ワールド、死亡タイマーの初期化
    mOwner = owner;
    pWorld = world;
    deadTimer = 0.0f;

    // 死亡時にメッシュのスケールを変更（高さを半分にする）
    owner->GetMesh()->SetRelativeScale3D(FVector(1.0f, 1.0f, 0.5f));

    // メッシュや攻撃判定が無ければ処理を終了
    if (!owner->GetMesh() || !owner->GetAttackCollision())
        return false;

    // メッシュのコリジョンを無効にする（死亡後は衝突しないように）
    owner->GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    owner->GetAttackCollision()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    // 新しいマテリアルを生成して設定
    UMaterialInterface* newMaterial = ALevelManager::GetInstance(GetWorld())->GetEnemyContainer()->CreateMaterial(world, materialID);
    if (!newMaterial)
        return false;

    // メッシュに新しいマテリアルを設定
    owner->GetMesh()->SetMaterial(0, newMaterial);

    return true; // 初期化が成功した場合
}

// ゴンバの死亡状態で毎フレーム実行される処理
bool UGoombaDeadState::OnUpdate(float deltaTime)
{
    // ワールドが無効であれば処理しない
    if (!pWorld)
        return false;

    // 死亡タイマーを更新
    deadTimer += pWorld->DeltaTimeSeconds;

    // 3秒以内は死亡状態を維持
    if (deadTimer < 3.0f)
        return true;

    // 3秒経過後にゴンバを破壊
    mOwner->Destroy();
    return true;
}

// ゴンバの死亡状態を終了する際の処理
bool UGoombaDeadState::OnExit()
{
    // 特に終了時の処理は必要ない（今後追加する場合は処理を記述）
    return true;
}

// 他のアクターとの衝突時の処理（死亡後は衝突処理を行わない）
bool UGoombaDeadState::OnOverlap(AActor*)
{
    return false; // 死亡状態では衝突処理は行わない
}
