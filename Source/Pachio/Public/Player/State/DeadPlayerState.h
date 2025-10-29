// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Player/PlayerStateComponent.h"
#include "DeadPlayerState.generated.h"

class APlayerCharacter;
class URespawnComponent;

UCLASS()
class PACHIO_API UDeadPlayerState : public UPlayerStateComponent
{
    GENERATED_BODY()

public:
    /**
     * @brief コンストラクタ。デッド状態用プレイヤーステートの初期化を行う
     */
    UDeadPlayerState();

    /**
     * @brief ステートに入ったときの処理
     *
     * @param Owner このステートが適用されるPawn
     * @return ステート遷移が成功した場合 true
     */
    virtual bool OnEnter(APawn* Owner) override;

    /**
     * @brief 毎フレームの更新処理（Tickの代わりに呼ばれる）
     *
     * @param DeltaTime 前フレームからの経過時間
     * @return 更新が正常に行われた場合 true
     */
    virtual bool OnUpdate(float DeltaTime) override;

    /**
     * @brief ステートから出るときの処理
     *
     * @param Owner このステートが適用されていたPawn
     * @return ステート終了が正常に行われた場合 true
     */
    virtual bool OnExit(APawn* Owner) override;

private:
    /** @brief 経過時間（秒） */
    float ElapsedTime;

    /** @brief すでにリスポーン済みかどうかのフラグ */
    bool bIsRespawned = false;

    /** @brief 対象プレイヤーへの弱参照（GC安全） */
    UPROPERTY()
    TWeakObjectPtr<APlayerCharacter> PlayerCharacter;

    /** @brief リスポーン用コンポーネントへの弱参照 */
    UPROPERTY()
    TWeakObjectPtr<URespawnComponent> RespawnComponent;

private:
    static constexpr float SETUP_POSITION_DELAYTIME = 1.5f;
    /** @brief リスポーンまでの遅延時間（秒） */
    static constexpr float RESPAWN_DELAY = 2.0f;
};
