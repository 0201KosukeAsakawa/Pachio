// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Player/PlayerStateComponent.h"
#include "PlayerDefaultState.generated.h"
class UMoveComponent;
class UBoxComponent;

UCLASS()
class UPlayerDefaultState : public UPlayerStateComponent
{
    GENERATED_BODY()

public:
    UPlayerDefaultState();

    /**
     * 状態が有効化された際に呼ばれる
     *
     * @param owner この状態の所有者となるPawn
     * @param world 実行中のWorld
     * @return 状態の初期化が成功したか
     */
    virtual bool OnEnter(APawn* owner) override;

    /**
     * 毎フレームの更新処理
     *
     * @param DeltaTime 経過時間
     * @return 状態を継続するか（falseで終了）
     */
    virtual bool OnUpdate(float DeltaTime) override;

    /**
     * 状態が終了する際に呼ばれる
     *
     * @param owner この状態の所有者となるPawn
     * @return 終了処理が正常に完了したか
     */
    virtual bool OnExit(APawn* owner) override;

    /**
     * スキル入力が行われた際に呼ばれる
     *
     * @param Value 入力値
     * @return スキルを実行したか
     */
    virtual bool OnSkill(const FInputActionValue& Value) override;

    /**
     * 移動入力を処理する
     *
     * @param Value 入力方向値
     */
    void Movement(const FInputActionValue& Value);

    /**
     * ジャンプ処理を実行する
     *
     * @param jumpForce ジャンプ力
     * @return ジャンプが成功したか
     */
    bool Jump(float jumpForce);

private:
    // ===== 初期化関連 =====
        // ===== 初期化関連 =====
    /**
     * 各種コンポーネントの初期化を行う
     *
     * @return すべての初期化が成功したか
     */
    bool InitializeComponents();

    /**
     * 移動コンポーネントを初期化する
     *
     * @return 初期化が成功したか
     */
    //bool InitializeMovementComponent();

    /**
     * 物理計算関連の初期化を行う
     *
     * @return 初期化が成功したか
     */
    //bool InitializePhysicsComponent();

    /**
     * 当たり判定コンポーネントを初期化する
     *
     * @return 初期化が成功したか
     */
    //bool InitializeHitBoxComponent();

    /**
     * プレイヤーのマテリアルを初期化する
     *
     * @param owner 対象となるPawn
     * @return マテリアル設定が成功したか
     */
    //bool InitializeMaterial(APawn* owner);

    /**
     * プレイヤーコントローラーを初期化する
     *
     * @return 初期化が成功したか
     */
    //bool InitializePlayerController();

    /**
     * CharacterMovementを初期化する
     *
     * @return 初期化が成功したか
     */
    //bool InitializeCharacterMovement();


    // ===== ジャンプ・着地関連 =====
    /**
     * ジャンプ中の状態更新を行う
     *
     * @param DeltaTime 経過時間
     */
    //void UpdateJumpState(float DeltaTime);

    /**
     * 着地時の共通処理を行う
     */
    //void HandleJumpLanding();

    /**
     * 通常着地時の処理を行う
     */
    //void HandleNormalLanding();

    /**
     * ジャンプ状態を終了すべきかを判定する
     *
     * @return ジャンプ状態を終了する必要がある場合はtrue
     */
    //bool ShouldExitJumpState() const;

    /**
     * ジャンプ状態をリセットする
     */
    void ResetJumpState();


    // ===== 移動関連 =====
    /**
     * 入力値から移動方向を算出する
     *
     * @param MoveInput 入力方向（X: 前後, Y: 左右）
     * @return ワールド空間での移動方向
     */
    //FVector CalculateMovementDirection(const FVector2D& MoveInput);

    /**
     * 入力方向に応じてキャラクターの回転を更新する
     *
     * @param direction 現在の移動方向
     * @param MoveInput 入力ベクトル
     */
    //void UpdateCharacterRotation(const FVector& direction, const FVector2D& MoveInput);

    /**
     * 入力の強さ（移動速度スケール）を算出する
     *
     * @return 移動速度スケール値
     */
    //float CalculateInputScale() const;

    /**
     * 実際の移動を適用する
     *
     * @param direction 移動方向
     */
    //void ApplyMovement(const FVector& direction);


    // ===== スキル関連 =====
    /**
     * スキルを発動できるか判定する
     *
     * @return 発動可能な場合はtrue
     */
    //bool CanActivateSkill() const;

    /**
     * 近くの持てるオブジェクトを検索する
     *
     * @return 持てるオブジェクト（存在しない場合はnullptr）
     */
    //AActor* FindHoldableObject() const;

    /**
     * 対象オブジェクトを持つ状態へ遷移を試みる
     *
     * @param Target 持つ対象となるアクター
     * @return 状態遷移が成功したか
     */
    //bool TryChangeToHoldState(AActor* Target);


    // ===== はしご関連 =====
    /**
     * ジャンプ中に梯子へ掴まれるかを判定する
     *
     * @return 掴まれる場合はtrue
     */
    bool TryEnterLadderOnJump() const;

    /**
     * 周囲の梯子を探索する
     *
     * @param OutLadders 発見した梯子アクターの配列
     * @return 一つ以上の梯子が見つかった場合はtrue
     */
    //bool FindOverlappingLadder(TArray<AActor*>& OutLadders) const;


    // ===== サウンド関連 =====
    /**
     * 着地音を再生する
     */
    //void PlayLandSound();

    /**
     * ジャンプ音を再生する
     */
    //void PlayJumpSound();


    // ===== ユーティリティ =====
    /**
     * CharacterMovementコンポーネントを取得する
     *
     * @return 取得したUCharacterMovementComponent
     */
    UCharacterMovementComponent* GetCharacterMovement() const;

    /**
     * 空中にいるかを判定する
     *
     * @return 空中状態であればtrue
     */
    bool IsInAir() const;

    /**
     * 地上にいるかを判定する
     *
     * @return 地上にいればtrue
     */
    bool IsOnGround() const;

private:
    // ===== メンバ変数 =====

    UPROPERTY()
    UMoveComponent* MoveComp;                  // 移動処理コンポーネント

    UPROPERTY()
    UPhysicsCalculator* Physics;               // 物理計算用コンポーネント

    UPROPERTY()
    UCapsuleComponent* HitBox;                 // 当たり判定用カプセル

    FVector CurrentDirection;                  // 現在の移動方向

    bool bIsJumping;                           // ジャンプ中フラグ
    
    float JumpStartTime;                       // ジャンプ開始時刻
    
    float JumpStartIgnoreDuration;             // ジャンプ直後の入力無視時間
};
