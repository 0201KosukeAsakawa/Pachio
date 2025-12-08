// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Color/ObjectColorComponent.h"
#include "DataContainer/ColorTargetTypes.h"
#include "ColorReactiveSwitch.generated.h"

class UBoxComponent;

/**
 * @brief 色に反応して動作を切り替えるスイッチコンポーネント
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PACHIO_API UColorReactiveSwitchComponent : public UObjectColorComponent
{
	GENERATED_BODY()

public:
	/**
	 * @brief コンストラクタ。デフォルト値設定
	 */
	UColorReactiveSwitchComponent();

public:
	/**
	 * @brief 初期化処理
	 * スイッチの初期状態やエフェクト設定を行う
	 */
	virtual void Initialize();

	/**
	 * @brief 色反応処理
	 * 指定された色に応じてスイッチの状態や動作を切り替える
	 * @param InColor 新しく適用された色
	 */
	virtual void ApplyColorWithMatching(const FLinearColor& InColor);

	/**
	 * @brief コンポーネント登録時処理
	 *
	 * アクターにアタッチされた直後（エディタ・実行時とも）に呼ばれます。
	 * 必要であれば、この段階で依存コンポーネントの取得や初期設定を行います。
	 * @note BeginPlay よりも早いタイミングで呼び出されます。
	 */
	void OnRegister()override;

private:
	/** スイッチの当たり判定用ボックス（必要なら） */
	UPROPERTY(VisibleAnywhere, Category = "Color Switch")
	TObjectPtr<UBoxComponent> BoxComponent;

	/** 二次エフェクト（補助的な反応タイプを指定） */
	UPROPERTY(EditAnywhere, Category = "Color Switch")
	EColorCategory SecondaryEffect;

	/** 二次エフェクトに対応する色 */
	UPROPERTY(EditAnywhere, Category = "Color Switch")
	FLinearColor SecondaryColor;
};
