// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "StateManager.generated.h"

class UPlayerStateComponent;




// ============================================================
// 状態管理インターフェース (State Manager Interface)
// 各プレイヤーの状態遷移や現在の状態管理を統一的に扱うための基底定義
// 実装クラスはこのインターフェースを継承し、状態制御ロジックを提供する
// ============================================================

// UInterface の定義
UINTERFACE(Blueprintable, MinimalAPI)
class UStateManager : public UInterface
{
    GENERATED_BODY()
};


/**
 * @brief プレイヤー状態管理のためのインターフェースクラス
 *
 * このインターフェースは、プレイヤーの状態遷移や更新処理を
 * 共通のインターフェースとして定義する。
 * 各種状態クラス（例: 通常・ホールド・クライムなど）を切り替える責務を持つ。
 */
class PACHIO_API IStateManager
{
    GENERATED_BODY()

public:

    // ============================================================
    // 基本初期化処理
    // ============================================================

    /**
     * @brief 状態マネージャーの初期化を行う
     *
     * @param Owner このマネージャーを所有する Pawn（通常はプレイヤーキャラクター）
     * @param World 所属するワールドオブジェクト
     *
     * @note
     * - 状態管理に必要な参照（プレイヤー、アニメーション、入力など）の設定を行う。
     * - ここでメッシュやアニメーションBPを差し替える実装も可能。
     */
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "StateManager")
    void Init(APawn* Owner, UWorld* World);
    virtual void Init_Implementation(APawn* Owner, UWorld* World) {}

    // ============================================================
    // 毎フレーム更新処理
    // ============================================================

    /**
     * @brief 状態マネージャーのフレーム更新処理
     *
     * @param DeltaTime 前フレームからの経過時間（秒）
     *
     * @note
     * - 現在アクティブな状態に対して更新処理（入力処理や移動制御など）を行う。
     * - 状態遷移条件の監視にも利用される。
     */
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "StateManager")
    void Update(float DeltaTime);
    virtual void Update_Implementation(float DeltaTime) {}

    // ============================================================
    // 状態遷移処理
    // ============================================================

    /**
     * @brief プレイヤーの状態を指定の状態に変更する
     *
     * @param NextStateTag 遷移先の状態タイプ（例: Hold, Climb, Dead）
     * @return 遷移後にアクティブとなった状態コンポーネントのポインタ
     *
     * @note
     * - 状態遷移時には、前の状態の終了処理 → 新状態の初期化を行う。
     */
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "StateManager")
    UPlayerStateComponent* ChangeState(EPlayerStateType NextStateTag);
    virtual UPlayerStateComponent* ChangeState_Implementation(EPlayerStateType NextStateTag) { return nullptr; }

    // ============================================================
    // 状態照会処理
    // ============================================================

    /**
     * @brief 現在の状態が指定した状態タイプと一致しているか確認する
     *
     * @param StateTag 照合対象の状態タイプ
     * @return 一致していれば true、不一致なら false
     *
     * @note
     * - 状態依存の入力制御やアクション許可判定などに利用される。
     */
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "StateManager")
    bool IsStateMatch(EPlayerStateType StateTag);
    virtual bool IsStateMatch_Implementation(EPlayerStateType StateTag) { return false; }

    // ============================================================
    // 現在状態取得処理
    // ============================================================

    /**
     * @brief 現在アクティブな状態コンポーネントを取得する
     *
     * @return 現在の状態を表す UPlayerStateComponent のポインタ（未設定時は nullptr）
     *
     * @note
     * - 現在の状態のロジック（移動・入力制御など）にアクセスしたい場合に使用する。
     */
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "StateManager")
    UPlayerStateComponent* GetCurrentState() const;
    virtual UPlayerStateComponent* GetCurrentState_Implementation() const { return nullptr; }
};