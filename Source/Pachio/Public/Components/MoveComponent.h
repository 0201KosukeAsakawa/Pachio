// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Interface/MoveLogic.h"
#include "MoveComponent.generated.h"


/**
 * 移動処理を管理するアクターコンポーネント
 *
 * アクターの移動方向・速度を制御し、外部の移動ロジック（IMoveLogic）を利用して移動挙動を拡張可能にします。
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PACHIO_API UMoveComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    /** コンストラクタ。デフォルト値を設定 */
    UMoveComponent();

    /**
     * 移動処理の初期化を行う
     *
     * @param MoveLogic 移動ロジックのインターフェース
     * @param Speed 移動速度
     * @param NewDirection 初期の移動方向
     */
    UFUNCTION(BlueprintCallable, Category = "Movement")
    void Init(TScriptInterface<IMoveLogic> MoveLogic, const float Speed = 1, const FVector NewDirection = FVector(0, 0, 0));

    /**
     * 移動処理を実行
     *
     * @param DeltaTime フレーム経過時間
     * @param Owner 所有アクター
     * @param Value 入力値（任意）
     *
     * @return 移動後の位置
     */
    UFUNCTION(BlueprintCallable, Category = "Movement")
    FVector Movement(float DeltaTime, AActor* Owner, const FInputActionValue& Value = FInputActionValue());

    /**
     * 移動方向を設定
     *
     * @param NewDirection 新しい移動方向
     */
    UFUNCTION(BlueprintCallable, Category = "Movement")
    void SetDirection(FVector NewDirection);

public:
    /**
     * 移動ロジックをセットする
     *
     * @param Logic 新たに設定する移動ロジック
     *
     * @return 設定に成功したかどうか
     */
    bool SetMoveLogic(TScriptInterface<IMoveLogic> Logic);

private:
    /** 移動速度 */
    float MoveSpeed;

    /** 移動ロジックのインターフェース */
    UPROPERTY(EditAnywhere)
    TScriptInterface<IMoveLogic> MoveLogic;

private:
    static constexpr float DEFAULT_MOVESPEED = 600.f;
};
