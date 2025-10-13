// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ColorUtilityLibrary.generated.h"
/**
 * HSL色空間の構造体
 */
USTRUCT(BlueprintType)
struct FHSLColor
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Color")
    float H = 0.0f;  // 色相 (0.0 - 1.0)

    UPROPERTY(BlueprintReadWrite, Category = "Color")
    float S = 0.0f;  // 彩度 (0.0 - 1.0)

    UPROPERTY(BlueprintReadWrite, Category = "Color")
    float L = 0.0f;  // 輝度 (0.0 - 1.0)
};

/**
 * 色計算の共通ユーティリティライブラリ
 * 静的関数として提供し、どこからでも使用可能
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PACHIO_API UColorUtilityLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    // =======================
    // 色空間変換
    // =======================

    /** RGB → HSL 変換 */
    UFUNCTION(BlueprintPure, Category = "Color|Conversion")
    static FHSLColor RGBToHSL(const FLinearColor& Color);

    /** HSL → RGB 変換 */
    UFUNCTION(BlueprintPure, Category = "Color|Conversion")
    static FLinearColor HSLToRGB(const FHSLColor& HSL);

    /** 補色を計算（パステル調整付き） */
    UFUNCTION(BlueprintPure, Category = "Color|Conversion")
    static FLinearColor GetComplementaryColor(const FLinearColor& InColor);

    // =======================
    // 色差計算
    // =======================

    /** 色相角度の差を計算（HSV空間、0〜180度） */
    UFUNCTION(BlueprintPure, Category = "Color|Distance")
    static float GetHueAngleDistance(const FLinearColor& ColorA, const FLinearColor& ColorB);

    /** RGB空間でのユークリッド距離 */
    UFUNCTION(BlueprintPure, Category = "Color|Distance")
    static float GetRGBDistance(const FLinearColor& ColorA, const FLinearColor& ColorB);

    /** 人間の視覚特性に基づいた重み付き色差（ITU-R BT.601係数） */
    UFUNCTION(BlueprintPure, Category = "Color|Distance")
    static float GetPerceptualColorDistance(const FLinearColor& ColorA, const FLinearColor& ColorB);

    // =======================
    // 色判定
    // =======================

    /** RGB距離が閾値内かを判定 */
    UFUNCTION(BlueprintPure, Category = "Color|Comparison")
    static bool IsRGBDistanceWithinThreshold(
        const FLinearColor& ColorA,
        const FLinearColor& ColorB,
        float Threshold);

    /** 知覚的色差が閾値内かを判定 */
    UFUNCTION(BlueprintPure, Category = "Color|Comparison")
    static bool IsPerceptualDistanceWithinThreshold(
        const FLinearColor& ColorA,
        const FLinearColor& ColorB,
        float Tolerance);

    /** 色が変更されたかを判定（デフォルト許容値使用） */
    UFUNCTION(BlueprintPure, Category = "Color|Comparison")
    static bool HasColorChanged(
        const FLinearColor& CurrentColor,
        const FLinearColor& CompareColor,
        float Tolerance = 0.1f);

    // =======================
    // 色調整
    // =======================

    /** 最大RGB成分を強調（エフェクト用） */
    UFUNCTION(BlueprintPure, Category = "Color|Adjustment")
    static FLinearColor EnhanceMaxComponent(
        const FLinearColor& Color,
        float Multiplier = 50.0f);

private:
    // =======================
    // 定数
    // =======================

    // ITU-R BT.601 係数（人間の視覚特性）
    static constexpr float RedWeight = 0.299f;
    static constexpr float GreenWeight = 0.587f;
    static constexpr float BlueWeight = 0.114f;
};