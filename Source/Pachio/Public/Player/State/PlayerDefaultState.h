// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Player/PlayerStateComponent.h"
#include "PlayerDefaultState.generated.h"
class UMoveComponent;
class UBoxComponent;

UCLASS(Blueprintable)
class PACHIO_API UPlayerDefaultState : public UPlayerStateComponent
{
    GENERATED_BODY()

public:
    /**
     * @brief コンストラクタ。デフォルト状態プレイヤーステートの初期化を行う
     */
    UPlayerDefaultState();

public:
    /**
     * @brief ステートに入ったときの処理
     *
     * @param Owner このステートが適用されるPawn
     * @param World ワールド参照
     * @return ステート遷移が成功した場合 true
     */
    bool OnEnter(APawn*, UWorld*) override;

    /**
     * @brief 毎フレームの更新処理
     *
     * @param DeltaTime 前フレームからの経過時間
     * @return 更新が正常に行われた場合 true
     */
    bool OnUpdate(float) override;

    /**
     * @brief ステートから出るときの処理
     *
     * @param Owner このステートが適用されていたPawn
     * @return ステート終了が正常に行われた場合 true
     */
    bool OnExit(APawn*) override;

    /**
     * @brief スキル入力処理（通常状態での特殊行動）
     *
     * @param Input 入力値
     * @return スキル実行が成功した場合 true
     */
    bool OnSkill(const FInputActionValue&) override;

    /**
     * @brief プレイヤーの通常移動処理
     *
     * @param Value 入力値（スティックやキー操作）
     */
    void Movement(const FInputActionValue& Value) override;

    /**
     * @brief ジャンプ処理
     *
     * @param jumpForce ジャンプ力
     * @return ジャンプ成功時 true
     */
    bool Jump(float jumpForce) override;

private:
    /**
     * @brief ジャンプ時にラダー登攀状態に遷移可能か確認する
     *
     * @return 登攀可能な場合 true
     */
    bool TryEnterLadderOnJump() const;

private:
    /** @brief ホールド可能判定チェック用タイマーハンドル */
    FTimerHandle CheckHoldableHandle;

    /** @brief プレイヤー移動コンポーネント */
    UPROPERTY()
    UMoveComponent* MoveComp;

    /** @brief 当たり判定用ボックスコンポーネント */
    UPROPERTY()
    UBoxComponent* BoxComp;

    /** @brief 移動方向 */
    float Direction;

    /** @brief 移動速度 */
    float MoveSpeed;

    /** @brief 初期回転が設定済みか */
    bool InitialRotationSet;

    /** @brief 前フレームでホールド可能か */
    bool bPrevCanHold;

    /** @brief 前フレームで登攀可能か */
    bool bPrevCanClim;

    /** @brief 初期回転 */
    FRotator InitialRotation;

    /** @brief 現在の移動方向ベクトル */
    FVector CurrentDirection;

    /** @brief 物理計算コンポーネント */
    UPROPERTY()
    UPhysicsCalculator* Physics;

    /** @brief カプセル当たり判定コンポーネント */
    UPROPERTY()
    UCapsuleComponent* HitBox;

private:
    /** @brief デフォルト移動速度 */
    static constexpr float DEFAULT_MOVESPEED = 100.0f;
};
