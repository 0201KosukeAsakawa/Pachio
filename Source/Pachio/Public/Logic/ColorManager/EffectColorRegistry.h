// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "DataContainer/EffectMatchResult.h"
#include "EffectColorRegistry.generated.h"

UCLASS()
class PACHIO_API UEffectColorRegistry : public UActorComponent
{
    GENERATED_BODY()

public:
    UEffectColorRegistry();

    // =======================
    // エフェクト色の取得
    // =======================

    /** エフェクトタイプに対応する色を取得 */
    UFUNCTION(BlueprintPure, Category = "Effect|Color")
    FLinearColor GetEffectColor(EBuffEffect Effect) const;

    /** 全てのエフェクト色マップを取得 */
    UFUNCTION(BlueprintPure, Category = "Effect|Color")
    const TMap<EBuffEffect, FLinearColor>& GetEffectColorMap() const { return EffectColorMap; }

    // =======================
    // エフェクト検索
    // =======================

    /** 色相ベースで最も近いエフェクトを検索 */
    UFUNCTION(BlueprintPure, Category = "Effect|Matching")
    FEffectMatchResult GetClosestEffectByHue(const FLinearColor& InputColor) const;

    /** RGB距離ベースで最も近いエフェクトを検索 */
    UFUNCTION(BlueprintPure, Category = "Effect|Matching")
    FEffectMatchResult GetClosestEffectByRGB(const FLinearColor& InputColor) const;

    // =======================
    // 色登録・更新
    // =======================

    /** エフェクト色を登録/更新 */
    UFUNCTION(BlueprintCallable, Category = "Effect|Color")
    void SetEffectColor(EBuffEffect Effect, const FLinearColor& Color);

    /** エフェクト色をリセット（デフォルト値に戻す） */
    UFUNCTION(BlueprintCallable, Category = "Effect|Color")
    void ResetToDefaultColors();

protected:
    /** エフェクトタイプと色のマッピング */
    UPROPERTY(EditAnywhere, Category = "Effect")
    TMap<EBuffEffect, FLinearColor> EffectColorMap;

private:
    /** デフォルトの色マップを初期化 */
    void InitializeDefaultColors();

    // =======================
    // 定数
    // =======================

    /** 明度補正の閾値 */
    static constexpr float ValueDifferenceThreshold = 0.5f;

    /** 明度補正の係数（度/明度差） */
    static constexpr float ValueCorrectionFactor = 60.0f;
};