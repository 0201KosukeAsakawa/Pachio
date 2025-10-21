// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "StateManager.generated.h"

class UPlayerStateComponent;

UENUM(BlueprintType)
enum class EPlayerStateType : uint8
{
    Default  UMETA(DisplayName = "Default"),
    Hold     UMETA(DisplayName = "Hold"),
    Climb    UMETA(DisplayName = "Climb"),
    Dead     UMETA(DisplayName = "Dead"),
    // 他のステートを追加...
};

// インターフェイスのUClass宣言 (BlueprintTypeも指定)
UINTERFACE(Blueprintable, MinimalAPI)
class UStateManager : public UInterface
{
    GENERATED_BODY()
};

/**
 * インターフェイス本体
 */
class PACHIO_API IStateManager
{
    GENERATED_BODY()

public:

    // Blueprintで呼び出せるようにするため UFUNCTION マクロを追加

    /** ゲーム開始時の初期化処理 */
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "StateManager")
    void Init(APawn* Owner, UWorld* World);
    virtual void Init_Implementation(APawn* Owner, UWorld* World) {}

    /** 毎フレーム更新処理 */
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "StateManager")
    void Update(float DeltaTime);
    virtual void Update_Implementation(float DeltaTime) {}

    /** ステート切替 */
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "StateManager")
    UPlayerStateComponent* ChangeState(EPlayerStateType NextStateTag);
    virtual UPlayerStateComponent* ChangeState_Implementation(EPlayerStateType NextStateTag) { return nullptr; }

    /** 現在のステートタグと一致するか */
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "StateManager")
    bool IsStateMatch(EPlayerStateType StateTag);
    virtual bool IsStateMatch_Implementation(EPlayerStateType StateTag) { return false; }

    /** 現在のアクティブステートを取得 */
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "StateManager")
    UPlayerStateComponent* GetCurrentState() const;
    virtual UPlayerStateComponent* GetCurrentState_Implementation() const { return nullptr; }
};