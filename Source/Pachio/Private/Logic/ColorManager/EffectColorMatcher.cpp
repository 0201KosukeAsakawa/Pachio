// Fill out your copyright notice in the Description page of Project Settings.


#include "Logic/ColorManager/EffectColorMatcher.h"

UEffectColorMatcher::UEffectColorMatcher()
{
    EffectColorMap = {
        { EBuffEffect::Green,  FLinearColor(0.65f, 1.00f, 0.78f, 1.0f) }, // パステルミント
        { EBuffEffect::Blue,   FLinearColor(0.65f, 0.78f, 1.00f, 1.0f) }, // パステルスカイブルー
        { EBuffEffect::Red,    FLinearColor(1.00f, 0.75f, 0.65f, 1.0f) }, // パステルサーモン
        { EBuffEffect::Yellow, FLinearColor(1.00f, 1.00f, 0.65f, 1.0f) }, // パステルイエロー
        { EBuffEffect::Aqua,   FLinearColor(0.65f, 1.00f, 1.00f, 1.0f) }, // パステルアクア
        { EBuffEffect::Purple, FLinearColor(1.00f, 0.65f, 1.00f, 1.0f) }  // パステルラベンダー
    };
}

FEffectMatchResult UEffectColorMatcher::GetClosestEffectByHue(const FLinearColor& InputColor)
{
    FEffectMatchResult result;

    float MinDistance = TNumericLimits<float>::Max();
    float MaxPossibleDistance = FMath::Sqrt(3.0f); // RGB距離の最大値（(1,1,1)と(0,0,0)の距離）

    EBuffEffect ClosestEffect = EBuffEffect::Red;

    for (const auto& Elem : EffectColorMap)
    {
        float Distance = GetColorDistanceRGB(InputColor, Elem.Value);

        UE_LOG(LogTemp, Log, TEXT("Comparing with %d: RGB Distance = %.4f"),
            static_cast<int32>(Elem.Key), Distance);

        if (Distance < MinDistance)
        {
            MinDistance = Distance;
            ClosestEffect = Elem.Key;
        }
    }

    // 距離が最大値に近いほど弱く、0に近いほど強い（逆スケール）
    float StrengthRatio = 1.0f - (MinDistance / MaxPossibleDistance);
    StrengthRatio = FMath::Clamp(StrengthRatio, 0.0f, 1.0f); // 念のため

    // 結果設定
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
