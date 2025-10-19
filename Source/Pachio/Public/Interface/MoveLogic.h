// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InputActionValue.h"
#include "MoveLogic.generated.h"

// ======================================================
// UMoveLogic - 移動処理インターフェース
// ======================================================
// 各キャラクターやエネミーの移動ロジックを統一的に扱うための
// 抽象インターフェース。これを実装することで、個別の移動処理
// （追従、徘徊、回避など）を切り替え可能にする。
// ======================================================

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UMoveLogic : public UInterface
{
	GENERATED_BODY()
};

/**
 * @brief 移動ロジックの共通インターフェースクラス。
 *
 * 移動アルゴリズム（追跡・徘徊など）を統一化し、キャラクターごとに
 * 独自の移動挙動を簡潔に実装できるようにする。
 */
class PACHIO_API IMoveLogic
{
	GENERATED_BODY()

public:

	/**
	 * @brief 移動処理を実行する。
	 *
	 * 毎フレーム呼び出され、入力や内部ロジックに基づき移動方向を計算する。
	 *
	 * @param DeltaTime 経過時間（フレーム間隔）。
	 * @param Owner この移動ロジックを利用するアクター。
	 * @param Value 入力値（スティック・キーなど）。省略時はデフォルト。
	 * @return 移動ベクトル（方向と移動量を含む）。
	 */
	virtual FVector Movement(float DeltaTime, AActor* Owner, const FInputActionValue& Value = FInputActionValue());

	/**
	 * @brief 移動ロジックの初期化を行う。
	 *
	 * スピードや初期移動方向を設定する際に使用する。
	 *
	 * @param Speed 初期移動速度（デフォルト: 1.0f）。
	 * @param Direction 初期移動方向ベクトル（デフォルト: FVector(0, 0, 0)）。
	 */
	virtual void Init(const float Speed = 1.f, const FVector& Direction = FVector(0, 0, 0));
};
