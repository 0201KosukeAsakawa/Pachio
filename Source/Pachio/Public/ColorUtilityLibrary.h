// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "DataContainer/ColorTargetTypes.h"
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
UCLASS()
class PACHIO_API UColorUtilityLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    // =======================
    // 基本: 色相角度の取得と操作
    // =======================

    /**
     * 色から色相角度を取得（0〜360度）
     *
     * @param Color 対象色
     * @return 色相角度（0=赤, 120=緑, 240=青）
     */
    UFUNCTION(BlueprintPure, Category = "Color|Hue")
    static float GetHue(const FLinearColor& Color);

    /**
     * 色相角度から色を生成（彩度・明度は最大）
     *
     * @param HueDegrees 色相角度（0〜360度）
     * @return 生成された色
     */
    UFUNCTION(BlueprintPure, Category = "Color|Hue")
    static FLinearColor FromHue(float HueDegrees);

    // =======================
    // 色相角度の差（距離）
    // =======================

    /**
     * 2色間の色相角度差を計算（0〜180度）
     * 色相環の最短距離を返す
     *
     * 例: 赤(0度) と 青(240度) → 120度（360-240=120の方が近い）
     *
     * @param ColorA 色A
     * @param ColorB 色B
     * @return 色相角度差（0〜180度）
     */
    UFUNCTION(BlueprintPure, Category = "Color|Comparison")
    static float GetHueAngleDistance(const FLinearColor& ColorA,
        const FLinearColor& ColorB);

    /**
     * 特定色からの色相角度差を計算
     *
     * 例: 「赤からどれくらい離れているか」を判定
     *
     * @param Color 対象色
     * @param ReferenceHue 基準となる色相角度（0=赤, 120=緑, 240=青）
     * @return 色相角度差（0〜180度）
     */
    UFUNCTION(BlueprintPure, Category = "Color|Comparison")
    static float GetHueDistanceFromAngle(const FLinearColor& Color,
        float ReferenceHue);

    /*
     *
     * 特定色からの色相角度差を計算
     *
     * 戻り値は割合が返ってきます
     *
     * @param Color 確かめる対象色
     * @param EColorCategory 基準となる色相角度（0=赤, 120=緑, 240=青）
     * @return その色とどれくらい離れているか(0～1)
   */
    UFUNCTION(BlueprintPure, Category = "Color|Comparison")
    static float GetColorRatio(const FLinearColor& ColorA,
        const FLinearColor& ColorB);

    /**
    * 特定色からの色相角度差を計算
    *
    * 例: 「赤からどれくらい離れているか」を判定
    *
    * @param Color 確かめる対象色
    * @param EColorCategory 基準となる色相角度（0=赤, 120=緑, 240=青）
    * @param 色の測定する範囲(大体前後60°がちょうどいいかも)
    * @return その色とどれくらい離れているか(0～1)
    */
    UFUNCTION(BlueprintPure, Category = "Color|Comparison")
    static float GetColorRatioWithTolerance(const FLinearColor& ColorA,
        const FLinearColor& ColorB, float Tolerance = 60);

    // =======================
    // 色相判定
    // =======================

    /**
     * 色相が類似しているかを判定
     *
     * @param ColorA 色A
     * @param ColorB 色B
     * @param ThresholdDegrees 閾値（度）デフォルト: 30度
     * @return 類似している場合true
     */
    UFUNCTION(BlueprintPure, Category = "Color|Comparison")
    static bool IsHueSimilar(const FLinearColor& ColorA,
        const FLinearColor& ColorB,
        float ThresholdDegrees = 30.0f);

    /**
     * 特定の色相範囲内にあるかを判定
     *
     * 例: 「赤系統の色か？」を判定
     *
     * @param Color 対象色
     * @param CenterHue 中心となる色相角度
     * @param RangeDegrees 許容範囲（±N度）
     * @return 範囲内の場合true
     */
    UFUNCTION(BlueprintPure, Category = "Color|Comparison")
    static bool IsHueInRange(const FLinearColor& Color,
        float CenterHue,
        float RangeDegrees = 30.0f);

    // =======================
    // 色相操作
    // =======================

    /**
     * 補色を計算（色相を180度回転）
     *
     * 例: 赤(0度) → シアン(180度)
     *     緑(120度) → マゼンタ(300度)
     *
     * @param InColor 入力色
     * @return 補色
     */
    UFUNCTION(BlueprintPure, Category = "Color|Conversion")
    static FLinearColor GetComplementaryColor(const FLinearColor& InColor);

    /**
     * 色相を回転
     *
     * @param InColor 入力色
     * @param RotationDegrees 回転角度（正で時計回り）
     * @return 回転後の色
     */
    UFUNCTION(BlueprintPure, Category = "Color|Conversion")
    static FLinearColor RotateHue(const FLinearColor& InColor,
        float RotationDegrees);

    /**
     * 色が赤・青・緑・黄のどれに最も近いかを判定
     *
     * @param Color 対象色
     * @return 最も近い基本色
     */
    UFUNCTION(BlueprintPure, Category = "Color|Comparison")
    static EColorCategory GetNearestPrimaryColor(const FLinearColor& Color);

    // =======================
    // エフェクト用ヘルパー
    // =======================

    /**
     * 最大RGB成分を強調（エフェクト用）
     * 色相は保持したまま、明るさを増幅
     */
    UFUNCTION(BlueprintPure, Category = "Color|Effects")
    static FLinearColor EnhanceMaxComponent(const FLinearColor& Color,
        float Multiplier = 50.0f);
};