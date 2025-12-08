// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InGameController.generated.h"

/**
 * 
 */
UCLASS()
class PACHIO_API AInGameController : public APlayerController
{
    GENERATED_BODY()

public:
    /**
     * @brief コンストラクタ。コントローラー初期化
     */
    AInGameController();

protected:
    /**
     * @brief 入力コンポーネントのセットアップ
     *
     * 入力アクションや軸入力をバインドする
     */
    virtual void SetupInputComponent() override;

public:
    /**
     * @brief 指定アクターへのポゼッション切り替え
     *
     * @param Target ポゼッション対象アクター
     */
    void TogglePossession(AActor* Target);

    /**
     * @brief 元のプレイヤーPawnにポゼッションを戻す
     */
    void ReturnToOriginalPlayer();

private:
    /** @brief 元々のプレイヤーPawn */
    UPROPERTY()
    APawn* OriginalPawn = nullptr;

    /** @brief 現在ポゼッション中かどうか */
    bool bIsPossessing = false;

    /** @brief ポゼッション対象を検出する最大距離 */
    UPROPERTY(EditDefaultsOnly)
    float DetectionDistance;

    /** @brief ポゼッション判定用のBoxサイズ（半分サイズ） */
    UPROPERTY(EditDefaultsOnly)
    FVector BoxHalfSize;
};
