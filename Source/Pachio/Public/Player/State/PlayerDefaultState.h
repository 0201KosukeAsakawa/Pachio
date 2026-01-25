// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Player/PlayerStateComponent.h"
#include "PlayerDefaultState.generated.h"
class UMoveComponent;
class UBoxComponent;

UENUM(BlueprintType)
enum class EColorAbsorbMode:uint8
{
    Absorb,   // 色を吸収する
    Paint      // 色は吸収しない（無視）
};

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

    void ChangePaintMode(EColorAbsorbMode m)override { mode = m; };

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



    /** 投射物のクラス */
    UPROPERTY(EditDefaultsOnly, Category = "Projectile")
    TSubclassOf<class AColorProjectile> ProjectileClass;

    /** 現在選択中の色 */
    UPROPERTY(EditAnywhere, Category = "Color")
    FLinearColor CurrentSelectedColor;

    /** 発射角度（度）- 水平から上向き */
    UPROPERTY(EditAnywhere, Category = "Projectile", meta = (ClampMin = "0", ClampMax = "90"))
    float LaunchAngle;

    /** 発射速度 */
    UPROPERTY(EditAnywhere, Category = "Projectile")
    float LaunchSpeed;

    UPROPERTY(EditAnywhere)
    EColorAbsorbMode mode;
};
