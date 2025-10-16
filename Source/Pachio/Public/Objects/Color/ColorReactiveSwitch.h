// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Objects/Color/ColorReactiveObject.h"
#include "DataContainer/EffectMatchResult.h"
#include "ColorReactiveSwitch.generated.h"

class UBoxComponent;

UCLASS()
class PACHIO_API AColorReactiveSwitch : public AColorReactiveObject
{
	GENERATED_BODY()

public:
	/**
	 * コンストラクタ。スイッチの基本設定を行う
	 */
	AColorReactiveSwitch();

	/**
	 * 初期化処理
	 * スイッチの初期状態やエフェクト設定を行う
	 */
	virtual void Initialize() override;

private:
	/**
	 * 色反応処理
	 * 指定された色に応じてスイッチの状態や動作を切り替える
	 *
	 * @param InColor 新しく適用された色
	 */
	virtual void ApplyColorWithMatching(const FLinearColor& InColor) override;

private:
	/** スイッチの当たり判定用コリジョンボックス */
	UPROPERTY(VisibleAnywhere)
	UBoxComponent* BoxComponent;

	/** 二次エフェクト（補助的な反応タイプを指定） */
	UPROPERTY(EditAnywhere)
	EColorCategory Second;

	/** 二次エフェクトに対応する色 */
	FLinearColor SecondColor;
};

