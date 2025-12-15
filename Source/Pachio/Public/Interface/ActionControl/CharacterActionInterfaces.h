#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "CharacterActionInterfaces.generated.h"

// ======================================================
// 操作系統合インターフェース
// 各種入力（移動・ジャンプ・アクション・色変更・UI 等）を
// 共通的に扱うための定義。
// ======================================================

UINTERFACE(MinimalAPI)
class UControllableInterface : public UInterface
{
    GENERATED_BODY()
};

/**
 * @brief プレイヤー・カメラ・UI などが入力操作を受け取るための統合インターフェース。
 */
class PACHIO_API IControllableInterface
{
    GENERATED_BODY()

public:

    // ----------------------------
    // 移動操作
    // ----------------------------

    /** 移動入力 */
    virtual void Movement(const FInputActionValue& Value) {}

    /** ジャンプ入力 */
    virtual void Jump(const FInputActionValue& Value) {}

    // ----------------------------
    // アクション操作
    // ----------------------------

    /** 攻撃・スキルなどのアクション */
    virtual void Action(const FInputActionValue& Value) {}

    // ----------------------------
    // カラーモード / 視点操作
    // ----------------------------

    /** 色変更 */
    virtual void ChangeColor(float Value) {}

    /** キャラクター視点へ変更 */
    virtual void ChangeCameraViewModeToCharacter() {}

    /** グリッド視点へ変更 */
    virtual void ChangeCameraViewModeToGrid() {}

    // ----------------------------
    // スティック操作
    // ----------------------------

    /** スティック移動入力 */
    virtual void OnStickMove(const FInputActionValue& Value) {}

    // ----------------------------
    // オプション / UI 操作
    // ----------------------------

    /** メニュー操作 */
    virtual void OpenMenu(const FInputActionValue& Value) {}
};