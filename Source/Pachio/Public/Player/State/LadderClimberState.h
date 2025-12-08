// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Player/PlayerStateComponent.h"
#include "LadderClimberState.generated.h"

class ALadderActor;
class UMoveComponent;
class UObjectColorComponent;
UCLASS()
class PACHIO_API ULadderClimberState : public UPlayerStateComponent
{
    GENERATED_BODY()

public:
    /**
     * @brief 登る対象のラダーを設定する
     *
     * @param Target 対象のラダーアクター
     */
    void SetTargetLadder(ALadderActor* Target);

    /**
     * @brief ラダー登攀中のプレイヤー移動処理
     *
     * @param Value 入力値（スティックやキー操作）
     */
    void Movement(const FInputActionValue& Value) override;

private:
    /**
     * @brief ステートに入ったときの処理
     *
     * @param Owner このステートが適用されるPawn
     * @param World ワールド参照
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

    /**
     * @brief スキル入力処理（ラダー登攀状態での特殊行動）
     *
     * @param Input 入力値
     * @return スキル実行が成功した場合 true
     */
    virtual bool OnSkill(const FInputActionValue& Input) override;

private:
    /** @brief 登る対象のラダーアクター */
    UPROPERTY()
    ALadderActor* Ladder;

    /** @brief プレイヤー移動コンポーネント */
    UPROPERTY()
    UMoveComponent* MoveComp;

    /** @brief カラー反応用コンポーネント（色判定対象） */
    UPROPERTY()
    UObjectColorComponent* targetComp;

    /** @brief ラダー中心位置 */
    FVector LadderCenterPosition;
};