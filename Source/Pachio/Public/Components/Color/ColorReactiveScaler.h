// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Color/ObjectColorComponent.h"
#include "ColorReactiveScaler.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PACHIO_API UColorReactiveScaler : public UObjectColorComponent
{
	GENERATED_BODY()
public:
	// Sets default values for this component's properties
	UColorReactiveScaler();

	/**
	 * @brief 実行開始時の初期化処理
	 *
	 * ゲーム開始時（またはアクターがスポーンした時）に呼び出されます。
	 * コンポーネントの初期化、マネージャー登録、マテリアル設定などを行うのに適しています。
	 */
	virtual void BeginPlay() override;

	/**
	 * @brief 色の反応処理
	 * 引数に応じてオブジェクトを拡縮する
	 *
	 * @param NewColor 新しく適用された色
	 */
	virtual void ApplyColorWithMatching(const FLinearColor& NewColor) override;


private:
	UPROPERTY(EditAnywhere)
	float MaxScale = 1;
	FVector DefaultScalse;
};
