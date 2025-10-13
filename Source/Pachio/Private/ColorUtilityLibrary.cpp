// Fill out your copyright notice in the Description page of Project Settings.


#include "ColorUtilityLibrary.h"

// =======================
// 色空間変換
// =======================

FHSLColor UColorUtilityLibrary::RGBToHSL(const FLinearColor& Color)
{
    const float R = Color.R, G = Color.G, B = Color.B;
    const float Max = FMath::Max3(R, G, B);
    const float Min = FMath::Min3(R, G, B);
    const float Delta = Max - Min;

    FHSLColor HSL;
    HSL.L = (Max + Min) * 0.5f;

    if (Delta == 0.0f)
    {
        HSL.H = 0.0f;
        HSL.S = 0.0f;
    }
    else
    {
        HSL.S = (HSL.L < 0.5f) ? (Delta / (Max + Min)) : (Delta / (2.0f - Max - Min));

        if (Max == R)
            HSL.H = (G - B) / Delta + (G < B ? 6.0f : 0.0f);
        else if (Max == G)
            HSL.H = (B - R) / Delta + 2.0f;
        else
            HSL.H = (R - G) / Delta + 4.0f;

        HSL.H /= 6.0f;
    }
    return HSL;
}

FLinearColor UColorUtilityLibrary::HSLToRGB(const FHSLColor& HSL)
{
    float R, G, B;

    if (HSL.S == 0.0f)
    {
        R = G = B = HSL.L;
    }
    else
    {
        auto HueToRGB = [](float p, float q, float t) -> float
            {
                if (t < 0.0f) t += 1.0f;
                if (t > 1.0f) t -= 1.0f;

                if (t < 1.0f / 6.0f) return p + (q - p) * 6.0f * t;
                if (t < 0.5f) return q;
                if (t < 2.0f / 3.0f) return p + (q - p) * (2.0f / 3.0f - t) * 6.0f;
                return p;
            };

        const float q = (HSL.L < 0.5f) ? (HSL.L * (1.0f + HSL.S)) : (HSL.L + HSL.S - HSL.L * HSL.S);
        const float p = 2.0f * HSL.L - q;

        R = HueToRGB(p, q, HSL.H + 1.0f / 3.0f);
        G = HueToRGB(p, q, HSL.H);
        B = HueToRGB(p, q, HSL.H - 1.0f / 3.0f);
    }

    return FLinearColor(R, G, B, 1.0f);
}

FLinearColor UColorUtilityLibrary::GetComplementaryColor(const FLinearColor& InColor)
{
    FHSLColor HSL = RGBToHSL(InColor);

    // 色相を180度反転
    constexpr float ComplementaryHueOffset = 0.5f;
    HSL.H += ComplementaryHueOffset;
    if (HSL.H > 1.0f) HSL.H -= 1.0f;

    // パステル調に調整
    constexpr float PastelSaturation = 0.3f;
    constexpr float MinPastelLightness = 0.8f;
    constexpr float MaxPastelLightness = 1.0f;

    HSL.S = PastelSaturation;
    HSL.L = FMath::Clamp(HSL.L, MinPastelLightness, MaxPastelLightness);

    FLinearColor Complementary = HSLToRGB(HSL);

    // 最大成分を1.0に強制
    const float MaxComponent = FMath::Max3(Complementary.R, Complementary.G, Complementary.B);
    constexpr float MaxComponentValue = 1.0f;
    constexpr float MinComponentValue = 0.8f;

    if (Complementary.R == MaxComponent) Complementary.R = MaxComponentValue;
    if (Complementary.G == MaxComponent) Complementary.G = MaxComponentValue;
    if (Complementary.B == MaxComponent) Complementary.B = MaxComponentValue;

    if (Complementary.R != MaxComponentValue)
        Complementary.R = FMath::Clamp(Complementary.R, MinComponentValue, MaxComponentValue);
    if (Complementary.G != MaxComponentValue)
        Complementary.G = FMath::Clamp(Complementary.G, MinComponentValue, MaxComponentValue);
    if (Complementary.B != MaxComponentValue)
        Complementary.B = FMath::Clamp(Complementary.B, MinComponentValue, MaxComponentValue);

    Complementary.A = 1.0f;
    return Complementary;
}

// =======================
// 色差計算
// =======================

float UColorUtilityLibrary::GetHueAngleDistance(const FLinearColor& ColorA, const FLinearColor& ColorB)
{
    const FLinearColor HSV_A = ColorA.LinearRGBToHSV();
    const FLinearColor HSV_B = ColorB.LinearRGBToHSV();
    const float HueA = HSV_A.R;
    const float HueB = HSV_B.R;
    const float Delta = FMath::Abs(HueA - HueB);
    const float Distance = FMath::Min(Delta, 360.0f - Delta);
    return Distance;
}

float UColorUtilityLibrary::GetRGBDistance(const FLinearColor& ColorA, const FLinearColor& ColorB)
{
    const float dR = ColorA.R - ColorB.R;
    const float dG = ColorA.G - ColorB.G;
    const float dB = ColorA.B - ColorB.B;
    return FMath::Sqrt(dR * dR + dG * dG + dB * dB);
}

float UColorUtilityLibrary::GetPerceptualColorDistance(const FLinearColor& ColorA, const FLinearColor& ColorB)
{
    const float dR = ColorA.R - ColorB.R;
    const float dG = ColorA.G - ColorB.G;
    const float dB = ColorA.B - ColorB.B;

    return RedWeight * dR * dR + GreenWeight * dG * dG + BlueWeight * dB * dB;
}

// =======================
// 色判定
// =======================

bool UColorUtilityLibrary::IsRGBDistanceWithinThreshold(
    const FLinearColor& ColorA,
    const FLinearColor& ColorB,
    float Threshold)
{
    return GetRGBDistance(ColorA, ColorB) <= Threshold;
}

bool UColorUtilityLibrary::IsPerceptualDistanceWithinThreshold(
    const FLinearColor& ColorA,
    const FLinearColor& ColorB,
    float Tolerance)
{
    const float Distance = GetPerceptualColorDistance(ColorA, ColorB);
    return Distance <= Tolerance * Tolerance;
}

bool UColorUtilityLibrary::HasColorChanged(
    const FLinearColor& CurrentColor,
    const FLinearColor& CompareColor,
    float Tolerance)
{
    return !IsPerceptualDistanceWithinThreshold(CurrentColor, CompareColor, Tolerance);
}

// =======================
// 色調整
// =======================

FLinearColor UColorUtilityLibrary::EnhanceMaxComponent(const FLinearColor& Color, float Multiplier)
{
    const float MaxRGB = FMath::Max3(Color.R, Color.G, Color.B);
    FLinearColor Enhanced = Color;

    if (Color.R == MaxRGB)
        Enhanced.R *= Multiplier;
    else if (Color.G == MaxRGB)
        Enhanced.G *= Multiplier;
    else
        Enhanced.B *= Multiplier;

    Enhanced.A = Color.A;
    return Enhanced;
}