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

    // 入力色の明度（HSVのV）
    FLinearColor InputHSV = InputColor.LinearRGBToHSV();
    float InputVal = FMath::Clamp(InputHSV.B, 0.0f, 1.0f);
    float MinDistance = TNumericLimits<float>::Max();
    EBuffEffect ClosestEffect = EBuffEffect::Red;

    for (const auto& Elem : EffectColorMap)
    {
        const FLinearColor& EffectColor = Elem.Value;

        // 明度取得
        FLinearColor EffectHSV = EffectColor.LinearRGBToHSV();
        float EffectVal = FMath::Clamp(EffectHSV.B, 0.0f, 1.0f);

        // 色相角度の差（0〜180度）
        float HueDistance = GetHueAngleDistance(InputColor, EffectColor);  // 角度で返す

        // 明度差が大きい場合は補正（角度ベースに変換して加算）
        float ValDiff = FMath::Abs(InputVal - EffectVal);
        if (ValDiff > 0.5f)
        {
            // 例：明度差0.5以上なら最大+30度補正
            HueDistance += (ValDiff - 0.5f) * 60.0f;  // 0〜30度加算
        }

        // 最小距離を更新
        if (HueDistance + KINDA_SMALL_NUMBER < MinDistance)
        {
            MinDistance = HueDistance;
            ClosestEffect = Elem.Key;
        }
    }

    result.ClosestEffect = ClosestEffect;
    result.Distance = MinDistance;  // 単位：度
    result.StrengthRatio = FMath::Clamp(1.0f - (MinDistance / 180.0f), 0.0f, 1.0f);  // 必要なら

    return result;
}


float UEffectColorMatcher::GetHueAngleDistance(const FLinearColor& A, const FLinearColor& B)
{
    FLinearColor HSV_A = A.LinearRGBToHSV();
    FLinearColor HSV_B = B.LinearRGBToHSV();

    float HueA = HSV_A.R;
    float HueB = HSV_B.R;

    float Delta = FMath::Abs(HueA - HueB);
    float Distance = FMath::Min(Delta, 360.0f - Delta);

    return Distance;
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
