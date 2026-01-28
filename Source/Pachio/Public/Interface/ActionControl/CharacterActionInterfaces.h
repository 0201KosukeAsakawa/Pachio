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

// IControllableInterfaceに以下のメソッドを追加
UINTERFACE(MinimalAPI, Blueprintable)
class UControllableInterface : public UInterface
{
    GENERATED_BODY()
};

class IControllableInterface
{
    GENERATED_BODY()

public:
    // 既存のメソッド...
    virtual void Movement(const FInputActionValue& Value) {}
    virtual void Jump(const FInputActionValue& Value) {}
    virtual void Action(const FInputActionValue& Value) {}
    virtual void ChangeColor(float Value) {}
    virtual void ChangeCameraViewModeToCharacter() {}
    virtual void ChangeCameraViewModeToGrid() {}
    virtual void OnStickMove(const FInputActionValue& Value) {}
    virtual void OpenMenu(const FInputActionValue& Value) {}
    virtual void SwitchColorTankNext(const FInputActionValue& Value) {}
    virtual void SwitchColorTankPrevious(const FInputActionValue& Value) {}
};