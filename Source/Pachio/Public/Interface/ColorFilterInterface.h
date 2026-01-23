// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "DataContainer/ColorTargetTypes.h"
#include "ColorFilterInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UColorReactive : public UInterface
{
	GENERATED_BODY()
};

class PACHIO_API IColorReactive
{
    GENERATED_BODY()

public:
    // =======================
    // カラーリアクティブ共通インターフェース
    // =======================

    /**
     * プレイヤーの色変更操作によるギミック起動
     * @param TriggerColor - トリガーとなった色
     */
    virtual void ActivateDirect(const FLinearColor& NewColor) {};

    /**
     * 外部トリガー（スイッチ等）による条件付きギミック起動
     * @param TriggerColor - トリガーとなった色（マッチング判定に使用）
     */
    virtual void ActivateConditional(const FLinearColor& TriggerColor) {};

    /**
     * 色の状態をリセットする
     *
     * @param MatchResult - エフェクトのマッチ結果情報
     */
    virtual void ResetColor() {};

    /**
     * 色が変更されたかを判定する
     *
     * @return 変更されていれば true
     */
    virtual bool HasColorChanged() const { return false; };

    /**
     * 色が変更可能かを判定する
     *
     * @return 変更可能であれば true
     */
    virtual bool IsChangeable() const { return false; };

    /**
     * 現在の色がマッチしているかを判定する
     *
     * @return マッチしていれば true
     */
    virtual bool IsColorMatched() const { return false; };

    /**
     * カラーイベントを識別するための ID を取得する
     *
     * @return カラーイベントの識別子（FName）
     */
    virtual FName GetColorEventID() const { return FName(); };
};
