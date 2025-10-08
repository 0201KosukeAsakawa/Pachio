// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/ControllableObjectBase.h"
#include "GameFramework/Character.h"
#include "Player/InGameController.h"


// =======================
// コンストラクタ
// =======================

AControllableObjectBase::AControllableObjectBase()
{
    // 特に初期化処理なし
}

// =======================
// 操作権切り替え
// =======================

// 指定されたプレイヤー(Pawn)のコントローラーをこのオブジェクトに切り替える
void AControllableObjectBase::SwitchControll(APawn* player)
{
    // この Pawn を操作しているコントローラーを取得
    AController* OwningController = player->GetController();
    if (OwningController)
    {
        // AInGameController にキャスト
        // （このコントローラーがプレイヤーキャラクターを Possess している場合）
        AInGameController* InGameController = Cast<AInGameController>(OwningController);
        if (InGameController)
        {
            // コントローラーの Possession を切り替える
            // → プレイヤーの操作対象をこのオブジェクトに移す
            InGameController->TogglePossession(this);
        }
    }
}

// =======================
// アクション入力処理
// =======================

// このオブジェクトを操作中にアクションが押されたら元のプレイヤーに戻る
void AControllableObjectBase::Action(const FInputActionValue& Value)
{
    // 入力が有効（ボタンが押されたなど）の場合のみ処理
    if (Value.Get<bool>())
    {
        // このオブジェクトを操作しているコントローラーを取得
        AController* OwningController = GetController();
        if (OwningController)
        {
            // AInGameController にキャスト
            // （現在このオブジェクトを Possess している場合）
            AInGameController* InGameController = Cast<AInGameController>(OwningController);
            if (InGameController)
            {
                // 元のプレイヤーキャラクターに操作権を戻す
                InGameController->ReturnToOriginalPlayer();
            }
        }
    }
}
