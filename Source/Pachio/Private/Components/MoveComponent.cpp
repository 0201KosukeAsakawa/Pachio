// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/MoveComponent.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "Components/PrimitiveComponent.h"
#include "GameFramework/PlayerController.h"
#include "Math/UnrealMathUtility.h"
#include "Interface/MoveLogic.h"

// ============================================================
//  UMoveComponent.cpp
//  移動処理を担当するアクターコンポーネントの実装
// ============================================================


/**
 * コンストラクタ
 *
 * コンポーネントのデフォルト値を設定します。
 * 初期速度は 600.0f に設定されています。
 */
UMoveComponent::UMoveComponent()
                                :MoveSpeed(DEFAULT_MOVESPEED)
{

}

/**
 * 移動ロジックの初期化を行う
 *
 * @param moveLogic 移動ロジックを保持するインターフェース
 * @param speed 移動速度
 * @param NewDirection 初期移動方向
 *
 * @note 渡された移動ロジックの Init を呼び出して初期化します。
 */
void UMoveComponent::Init(TScriptInterface<IMoveLogic> moveLogic, const float speed, const FVector NewDirection)
{
    MoveSpeed = speed;
    MoveLogic = moveLogic;
    MoveLogic->Init(speed, NewDirection);
}

/**
 * 毎フレームごとの移動処理を実行
 *
 * @param DeltaTime フレーム経過時間
 * @param Owner このコンポーネントを持つアクター
 * @param Value 入力値（移動方向やスティック値など）
 *
 * @return 計算された移動ベクトル。ロジックが存在しない場合は (0, 0, 0)
 *
 * @note IMoveLogic に定義された Movement() を呼び出して、移動量を取得します。
 */
FVector UMoveComponent::Movement(float DeltaTime, AActor* Owner, const FInputActionValue& Value)
{
    if (!MoveLogic)
    {
        return FVector(0, 0, 0);
    }
    return MoveLogic->Movement(DeltaTime, Owner, Value);
}

/**
 * 移動方向を設定
 *
 * @param NewDirection 新しい移動方向
 *
 * @note 既存の MoveLogic に対して Init を呼び出し、方向を更新します。
 */
void UMoveComponent::SetDirection(FVector NewDirection)
{
    MoveLogic->Init(MoveSpeed, NewDirection);
}

/**
 * 新しい移動ロジックを設定
 *
 * @param Logic 設定する IMoveLogic インターフェース
 *
 * @return 設定に成功した場合 true、失敗した場合 false
 */
bool UMoveComponent::SetMoveLogic(TScriptInterface<IMoveLogic> Logic)
{
    if (!Logic)
        return false;

    MoveLogic = Logic;
    return true;
}
