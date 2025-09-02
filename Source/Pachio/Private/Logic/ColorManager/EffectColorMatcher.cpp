// Fill out your copyright notice in the Description page of Project Settings.


#include "Logic/ColorManager/EffectColorMatcher.h"

UEffectColorMatcher::UEffectColorMatcher()
{
    EffectColorMap = {
        { EBuffEffect::Green,  FLinearColor(0.65f, 1.00f, 0.78f, 1.0f) }, // パステルミント
        { EBuffEffect::Blue,   FLinearColor(0.65f, 0.78f, 1.00f, 1.0f) }, // パステルスカイブルー
        { EBuffEffect::Red,    FLinearColor(1.00f, 0.75f, 0.65f, 1.0f) }, // パステルサーモン
        { EBuffEffect::Yellow, FLinearColor(1.00f, 1.00f, 0.65f, 1.0f) }, // パステルイエロー
         { EBuffEffect::Black,  FLinearColor(0.f, 0.f, 0.f, 1.0f) }, // パステルミント
    };
}

FEffectMatchResult UEffectColorMatcher::GetClosestEffectByHue(const FLinearColor& InputColor)
{
    FEffectMatchResult result;

    // 入力色をHSVに変換
    FLinearColor InputHSV = InputColor.LinearRGBToHSV();
    float InputHue = FMath::Fmod(InputHSV.R + 360.0f, 360.0f); // 0～360度に正規化
    float InputSat = FMath::Clamp(InputHSV.G, 0.0f, 1.0f);
    float InputVal = FMath::Clamp(InputHSV.B, 0.0f, 1.0f);

    float MinDistance = TNumericLimits<float>::Max();
    EBuffEffect ClosestEffect = EBuffEffect::Red;

    // 重み（Hue, Saturation, Value）
    const float HueWeight = 1.0f;
    const float SatWeight = 1.0f;
    const float ValWeight = 2.0f; // 明度差を強調

    for (const auto& Elem : EffectColorMap)
    {
        FLinearColor EffectHSV = Elem.Value.LinearRGBToHSV();
        float EffectHue = FMath::Fmod(EffectHSV.R + 360.0f, 360.0f);
        float EffectSat = FMath::Clamp(EffectHSV.G, 0.0f, 1.0f);
        float EffectVal = FMath::Clamp(EffectHSV.B, 0.0f, 1.0f);

        // Hue差を0~180で正規化
        float HueDiff = FMath::Abs(InputHue - EffectHue);
        HueDiff = FMath::Min(HueDiff, 360.0f - HueDiff) / 180.0f; // 0~1
        float SatDiff = FMath::Abs(InputSat - EffectSat);           // 0~1
        float ValDiff = FMath::Abs(InputVal - EffectVal);           // 0~1

        // 重み付き距離
        float Distance = (HueDiff * HueWeight + SatDiff * SatWeight + ValDiff * ValWeight) / (HueWeight + SatWeight + ValWeight);

        // 明度差が大きすぎる場合は距離を最大化（線形補正）
        if (ValDiff > 0.5f)
        {
            Distance = FMath::Clamp(Distance + (ValDiff - 0.5f), 0.0f, 1.0f);
        }

        UE_LOG(LogTemp, Log, TEXT("Comparing %d: HueDiff=%.3f SatDiff=%.3f ValDiff=%.3f WeightedDistance=%.3f"),
            static_cast<int32>(Elem.Key), HueDiff, SatDiff, ValDiff, Distance);

        // 浮動小数点誤差を考慮して最小距離更新
        if (Distance + KINDA_SMALL_NUMBER < MinDistance)
        {
            MinDistance = Distance;
            ClosestEffect = Elem.Key;
        }
    }

    // 距離を StrengthRatio に変換
    float StrengthRatio = 1.0f - MinDistance;
    StrengthRatio = FMath::Clamp(StrengthRatio, 0.0f, 1.0f);
    UE_LOG(LogTemp, Log, TEXT("MinDistance %f"), MinDistance);
    result.ClosestEffect = ClosestEffect;
    result.Distance = MinDistance;
    result.StrengthRatio = StrengthRatio;

    return result;
}


float UEffectColorMatcher::GetColorDistanceRGB(const FLinearColor& A, const FLinearColor& B)
{
    return FMath::Sqrt(
        FMath::Square(A.R - B.R) +
        FMath::Square(A.G - B.G) +
        FMath::Square(A.B - B.B)
    );
}

FLinearColor UEffectColorMatcher::GetEffectColor(EBuffEffect effect) const
{
    if (const FLinearColor* FoundColor = EffectColorMap.Find(effect))
    {
        return *FoundColor; // マップに存在すればその色を返す
    }

    // 見つからなかった場合はデフォルト値（白など）を返す
    return FLinearColor::White;
}
