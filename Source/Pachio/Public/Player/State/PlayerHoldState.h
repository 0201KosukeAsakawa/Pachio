// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Player/PlayerStateComponent.h"
#include "PlayerHoldState.generated.h"

class UMoveComponent;
class UColorReactiveComponent;
UCLASS()
class PACHIO_API UPlayerHoldState : public UPlayerStateComponent
{
    GENERATED_BODY()

public:
    /**
     * @brief コンストラクタ。ホールド状態プレイヤーステートの初期化
     */
    UPlayerHoldState();

    /**
     * @brief ホールド対象の設定
     *
     * @param Target ホールドする対象のアクター
     * @param bInitialGrab 初回ホールド時かどうか
     */
    void SetUp(AActor* Target, bool bInitialGrab);

private:
    /**
     * @brief ステートに入ったときの処理
     *
     * @param owner このステートが適用されるPawn
     * @param world ワールド参照
     * @return ステート遷移が成功した場合 true
     */
    bool OnEnter(APawn* owner) override;

    /**
     * @brief 毎フレームの更新処理（Tickの代わりに呼ばれる）
     *
     * @param DeltaTime 前フレームからの経過時間
     * @return 更新が正常に行われた場合 true
     */
    bool OnUpdate(float DeltaTime) override;

    /**
     * @brief ステートから出るときの処理
     *
     * @param owner このステートが適用されていたPawn
     * @return ステート終了が正常に行われた場合 true
     */
    bool OnExit(APawn* owner) override;

    /**
     * @brief スキル入力処理（ホールド状態での特殊行動）
     *
     * @param Value 入力値
     * @return スキル実行が成功した場合 true
     */
    bool OnSkill(const FInputActionValue& Value) override;

    /**
     * @brief ホールド状態での移動処理
     *
     * @param Value 入力値（スティックやキー操作）
     */
    void Movement(const FInputActionValue& Value) override;

private:
    /** @brief ホールド対象のアクター */
    UPROPERTY()
    AActor* HoldTarget;

    /** @brief プレイヤー移動コンポーネント */
    UPROPERTY()
    UMoveComponent* MoveComp;

    /** @brief 色反応判定用コンポーネント */
    UPROPERTY()
    UColorReactiveComponent* targetComp;

    /** @brief ホールド開始時の距離 */
    float InitialHoldDistance;

    /** @brief ホールド方向（左右の方向判定などに使用） */
    int32 GrabDirection;
};
