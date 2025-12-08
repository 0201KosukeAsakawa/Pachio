#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "CharacterActionInterfaces.generated.h"

// ======================================================
// 操作系インターフェース群
// 各種入力（移動・ジャンプ・アクション・色変更など）を
// プレイヤーキャラクターや制御オブジェクトに共通化するための定義。
// ======================================================

// ----------------------------
// 移動操作インターフェース
// ----------------------------
UINTERFACE(MinimalAPI)
class UControllableMover : public UInterface
{
	GENERATED_BODY()
};

/**
 * @brief プレイヤーなどが移動操作を行うためのインターフェース。
 *        入力値を受け取り、移動処理を実行する。
 */
class PACHIO_API IControllableMover
{
	GENERATED_BODY()

public:

	/**
	 * @brief 入力値に基づいてキャラクターを移動させる。
	 *
	 * @param Value 入力アクション値（方向やスティック入力など）
	 */
	virtual void Movement(const FInputActionValue& Value);
};


// ----------------------------
// ジャンプ操作インターフェース
// ----------------------------
UINTERFACE(MinimalAPI)
class UControllableJumper : public UInterface
{
	GENERATED_BODY()
};

/**
 * @brief ジャンプ操作を定義するインターフェース。
 */
class PACHIO_API IControllableJumper
{
	GENERATED_BODY()

public:

	/**
	 * @brief 入力に応じてジャンプを実行する。
	 *
	 * @param Value 入力アクション値（ボタン押下など）
	 */
	virtual void Jump(const FInputActionValue& Value);
};


// ----------------------------
// アクション操作インターフェース
// ----------------------------
UINTERFACE(MinimalAPI)
class UControllableAbility : public UInterface
{
	GENERATED_BODY()
};

/**
 * @brief 攻撃やスキルなど、特殊アクション操作を定義するインターフェース。
 */
class PACHIO_API IControllableAbility
{
	GENERATED_BODY()

public:

	/**
	 * @brief 入力に応じたアクションを実行する。
	 *
	 * @param Value 入力アクション値（ボタン入力など）
	 */
	virtual void Action(const FInputActionValue& Value);
};


// ----------------------------
// カラーモード操作インターフェース
// ----------------------------
UINTERFACE(MinimalAPI)
class UColorModeController : public UInterface
{
	GENERATED_BODY()
};

/**
 * @brief 色の切り替えや循環操作を行うインターフェース。
 */
class PACHIO_API IColorModeController
{
	GENERATED_BODY()

public:

	/**
	 * @brief 現在のカラーモードを変更する。
	 *
	 * @param Value 色変更用のスカラー値（増減量やトリガー値など）
	 */
	virtual void ChangeColor(float Value);

	/**
	 * @brief カラーモード配列を右方向（次の色）にシフトする。
	 */
	virtual void ChangeCameraViewModeToCharacter();

	/**
	 * @brief カラーモード配列を左方向（前の色）にシフトする。
	 */
	virtual void ChangeCameraViewModeToGrid();
};


// ----------------------------
// スティック操作インターフェース
// ----------------------------
UINTERFACE(MinimalAPI)
class UStickAction : public UInterface
{
	GENERATED_BODY()
};

/**
 * @brief スティック入力に対応する操作インターフェース。
 */
class PACHIO_API IStickAction
{
	GENERATED_BODY()

public:
	/**
	 * @brief スティックの入力に応じてキャラクターやカメラを操作する。
	 *
	 * @param Value スティック入力値（方向や強度を含む）
	 */
	virtual void OnStickMove(const FInputActionValue& Value);
};


// ----------------------------
// オプション操作インターフェース
// ----------------------------
UINTERFACE(MinimalAPI)
class UOptionAction : public UInterface
{
	GENERATED_BODY()
};

/**
 * @brief メニュー操作など、オプション関連のアクションを定義するインターフェース。
 */
class PACHIO_API IOptionAction
{
	GENERATED_BODY()

public:
	/**
	 * @brief オプションメニューを開く、またはトグルする。
	 *
	 * @param Value 入力アクション値（メニュー開閉ボタンなど）
	 */
	virtual void OpenMenu(const FInputActionValue& Value);
};
