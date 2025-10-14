// Fill out your copyright notice in the Description page of Project Settings.


#include "Logic/ColorManager/EffectColorRegistry.h"
#include "ColorUtilityLibrary.h"

UEffectColorRegistry::UEffectColorRegistry()
{
    InitializeDefaultColors();
}

// =======================
// 初期化
// =======================

void UEffectColorRegistry::InitializeDefaultColors()
{
    EffectColorMap = {
        { EBuffEffect::Green,  FLinearColor(0.65f, 1.00f, 0.78f, 1.0f) }, // パステルミント
        { EBuffEffect::Blue,   FLinearColor(0.65f, 0.78f, 1.00f, 1.0f) }, // パステルスカイブルー
        { EBuffEffect::Red,    FLinearColor(1.00f, 0.75f, 0.65f, 1.0f) }, // パステルサーモン
        { EBuffEffect::Yellow, FLinearColor(1.00f, 1.00f, 0.65f, 1.0f) }, // パステルイエロー
        { EBuffEffect::Black,  FLinearColor(0.00f, 0.00f, 0.00f, 1.0f) }, // ブラック
    };
}

void UEffectColorRegistry::ResetToDefaultColors()
{
    InitializeDefaultColors();
}

// =======================
// エフェクト色の取得
// =======================

FLinearColor UEffectColorRegistry::GetEffectColor(EBuffEffect Effect) const
{
    if (const FLinearColor* FoundColor = EffectColorMap.Find(Effect))
    {
        return *FoundColor;
    }

    // 見つからない場合はデフォルト値（白）を返す
    UE_LOG(LogTemp, Warning, TEXT("Effect color not found for effect type: %d"), static_cast<int32>(Effect));
    return FLinearColor::White;
}

// =======================
// エフェクト検索
// =======================

FEffectMatchResult UEffectColorRegistry::GetClosestEffectByHue(const FLinearColor& InputColor) const
{
    FEffectMatchResult Result;

    // 入力色の明度（HSVのV）
    const FLinearColor InputHSV = InputColor.LinearRGBToHSV();
    const float InputVal = FMath::Clamp(InputHSV.B, 0.0f, 1.0f);

    float MinDistance = TNumericLimits<float>::Max();
    EBuffEffect ClosestEffect = EBuffEffect::Red;

    for (const auto& Elem : EffectColorMap)
    {
        const EBuffEffect CurrentEffect = Elem.Key;
        const FLinearColor& EffectColor = Elem.Value;

        // 明度取得
        const FLinearColor EffectHSV = EffectColor.LinearRGBToHSV();
        const float EffectVal = FMath::Clamp(EffectHSV.B, 0.0f, 1.0f);

        // 色相角度の差（0〜180度）
        float HueDistance = UColorUtilityLibrary::GetHueAngleDistance(InputColor, EffectColor);

        // 明度差が大きい場合は補正（角度ベースに変換して加算）
        const float ValDiff = FMath::Abs(InputVal - EffectVal);
        if (ValDiff > ValueDifferenceThreshold)
        {
            // 明度差0.5以上なら最大+30度補正
            HueDistance += (ValDiff - ValueDifferenceThreshold) * ValueCorrectionFactor;
        }

        // 最小距離を更新
        if (HueDistance + KINDA_SMALL_NUMBER < MinDistance)
        {
            MinDistance = HueDistance;
            ClosestEffect = CurrentEffect;
        }
    }

    Result.ClosestEffect = ClosestEffect;
    Result.Distance = MinDistance;  // 単位：度
    Result.StrengthRatio = FMath::Clamp(1.0f - (MinDistance / 180.0f), 0.0f, 1.0f);

    return Result;
}

FEffectMatchResult UEffectColorRegistry::GetClosestEffectByRGB(const FLinearColor& InputColor) const
{
    FEffectMatchResult Result;

    float MinDistance = TNumericLimits<float>::Max();
    EBuffEffect ClosestEffect = EBuffEffect::Red;

    for (const auto& Elem : EffectColorMap)
    {
        const EBuffEffect CurrentEffect = Elem.Key;
        const FLinearColor& EffectColor = Elem.Value;

        // RGB距離を計算
        const float Distance = UColorUtilityLibrary::IsHueSimilar(InputColor, EffectColor);

        if (Distance < MinDistance)
        {
            MinDistance = Distance;
            ClosestEffect = CurrentEffect;
        }
    }

    Result.ClosestEffect = ClosestEffect;
    Result.Distance = MinDistance;
    Result.StrengthRatio = FMath::Clamp(1.0f - MinDistance, 0.0f, 1.0f);
    //Result.RGBThreshold = MinDistance * 1.2f;  // 閾値は距離の1.2倍

    return Result;
}

// =======================
// 色登録・更新
// =======================

void UEffectColorRegistry::SetEffectColor(EBuffEffect Effect, const FLinearColor& Color)
{
    EffectColorMap.Add(Effect, Color);
}