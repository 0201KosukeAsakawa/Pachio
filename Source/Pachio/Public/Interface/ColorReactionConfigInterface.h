// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ColorReactionConfigInterface.generated.h"


// ======================================================
// このインターフェースは、オブジェクトの色変化やロック状態、
// 反応イベントの管理を行うための統一的なインターフェースです。
// ======================================================

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UColorReactionConfigInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * @brief 色反応設定用インターフェース。
 *
 * オブジェクトのロック状態切り替え、色一致判定、イベントID取得など、
 * カラーリアクションの基本動作を定義する。
 */
class PACHIO_API IColorReactionConfigInterface
{
	GENERATED_BODY()

public:

	/**
	 * @brief オブジェクトのロック状態を切り替える。
	 *
	 * ロック中は色変更や反応を無効化するなどの制御を行うために使用。
	 *
	 * @param bIsLocked true の場合はロック、false の場合はロック解除。
	 */
	virtual void ChangeLock(bool bIsLocked);

	/**
	 * @brief 現在の色が一致しているかを判定する。
	 *
	 * @return 一致している場合は true、そうでなければ false。
	 */
	virtual bool IsColorMatch() const;

	/**
	 * @brief 色関連のイベントIDを取得する。
	 *
	 * 例えば「色が一致したときに発火するイベント」を識別するために利用。
	 *
	 * @return 対応するイベントの FName（識別子）。
	 */
	virtual FName GetColorEventID() const;
};
