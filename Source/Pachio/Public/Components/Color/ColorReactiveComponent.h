// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DataContainer/EffectMatchResult.h"
#include "ColorReactiveComponent.generated.h"


class ANiagaraActor;
class UNiagaraSystem;
class UNiagaraComponent;

// HSL色表現用の構造体
struct FHSLColor
{
	float H; // Hue (0.0〜1.0)
	float S; // Saturation (0.0〜1.0)
	float L; // Lightness (0.0〜1.0)
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PACHIO_API UColorReactiveComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// コンストラクタ
	UColorReactiveComponent();

	// --- void 関数 ---

	// 初期化。bSetStartColorがfalseなら無効化
	virtual void Init(bool bImmediate);

	// 色効果とNiagaraエフェクトを初期設定
	void InitColorEffectAndNiagara(const FLinearColor& FilterColor, EBuffEffect NewEffect, TArray<ANiagaraActor*> NiagaraActors);

	// マテリアルに色を適用
	void ApplyColorToMaterial(FLinearColor InColor);

	// 毎フレームの処理
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// 選択状態のセット
	void SetSelectMode(bool bIsNowSelected);

	// Niagaraエフェクトの有効/無効切り替え
	void ActiveEffect(bool bActive);

	// --- bool 関数 ---

	// 色の距離をRGB空間で判定。補色使用可
	bool IsRGBDistancewithinThreshold(FEffectMatchResult Result, const FLinearColor& FilterColor, const bool bUseComplementaryColor = false);

	// 色の距離を「色相の角度(degree)」で判定（Blueprint呼び出し可）
	UFUNCTION(BlueprintCallable)
	virtual bool IsColorDegreeDistanceWithinThreshold(const FLinearColor& FilterColor, const float Tolerance = 0.08f) const;

	// 上記のオーバーロード（対象色を指定）
	bool IsColorDegreeDistanceWithinThreshold(const FLinearColor& FilterColor, const FLinearColor& TargetColor, const float Tolerance = 0.08f) const;

	// 表示状態の取得
	inline bool IsHidden() const { return bHide; }

protected:
	// --- void 関数 ---

	// Niagaraアクターの表示切り替え処理
	void ToggleNiagaraActiveState(bool bVisible);

	// 出現時エフェクトの再生
	void PlayAppearEffect();

	// 指定Niagaraシステムをアタッチしてアクティブ化
	void ActiveNiagaraEffect(UNiagaraSystem* NiagaraSystem);

	// 全エフェクトの非アクティブ化
	void DeactivateAllEffects();

	// --- bool 関数 ---

	// 色が一致したときの処理（派生クラスでオーバーライド可能）
	UFUNCTION(BlueprintCallable)
	virtual bool OnColorMatched(const FLinearColor& FilterColor);

	// 色が不一致だったときの処理（派生クラスでオーバーライド可能）
	virtual bool OnColorMismatched(const FLinearColor& FilterColor);

	FLinearColor GetComplementaryColor(const FLinearColor& InColor);

protected:
	// --- メンバ変数 ---

	// 初期色設定フラグ
	UPROPERTY(EditAnywhere)
	bool bSetStartColor = true;

	// 現在の効果種別
	EBuffEffect Effect;

	// 現在の色
	UPROPERTY(EditAnywhere)
	FLinearColor CurrentColor;

	// 関連するNiagaraアクター群
	UPROPERTY()
	TArray<ANiagaraActor*> Niagaras;

	// Dynamic Material Instance（メッシュ用）
	UPROPERTY()
	UMaterialInstanceDynamic* DynMesh = nullptr;

	// Dynamic Material Instance（別途？）
	UPROPERTY()
	UMaterialInstanceDynamic* DynamicMaterialInstance = nullptr;

	// 使用するNiagaraシステム群
	UPROPERTY()
	UNiagaraSystem* FireflyBurstNiagaraSystem = nullptr;

	UPROPERTY()
	UNiagaraSystem* ParticlesOfLightNiagaraSystem = nullptr;

	UPROPERTY()
	UNiagaraSystem* LightCubeNiagaraSystem = nullptr;

	// アクティブなNiagaraコンポーネントの管理用配列
	UPROPERTY()
	TArray<UNiagaraComponent*> ActiveNiagaraComponent;

	// 選択状態
	bool bSelected = false;

	// 非表示フラグ
	bool bHide = false;
};
