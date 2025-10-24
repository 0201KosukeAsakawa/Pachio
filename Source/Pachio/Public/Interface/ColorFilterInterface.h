// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "DataContainer/ColorTargetTypes.h"
#include "ColorFilterInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UColorReactiveInterface : public UInterface
{
	GENERATED_BODY()
};

class PACHIO_API IColorReactiveInterface
{
    GENERATED_BODY()

public:
    // =======================
    // カラーリアクティブ共通インターフェース
    // =======================

    /**
     * 指定された色を適用し、必要に応じてマッチング処理を行う
     *
     * @param NewColor - 適用する新しい色
     */
    virtual void ApplyColorWithMatching(const FLinearColor& NewColor);

    /**
     * 色の状態をリセットする
     *
     * @param MatchResult - エフェクトのマッチ結果情報
     */
    virtual void ResetColor();

    /**
     * 選択状態を設定する
     *
     * @param bIsSelected - 選択状態にする場合は true
     */
    virtual void SetSelected(bool bIsSelected);

    /**
     * 色が変更されたかを判定する
     *
     * @return 変更されていれば true
     */
    virtual bool HasColorChanged() const;

    /**
     * 色が変更可能かを判定する
     *
     * @return 変更可能であれば true
     */
    virtual bool IsChangeable() const;

    /**
     * 現在の色がマッチしているかを判定する
     *
     * @return マッチしていれば true
     */
    virtual bool IsColorMatched() const;

    /**
     * カラーイベントを識別するための ID を取得する
     *
     * @return カラーイベントの識別子（FName）
     */
    virtual FName GetColorEventID() const;
};
