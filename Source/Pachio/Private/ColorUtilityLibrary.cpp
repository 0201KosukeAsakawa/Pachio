// Fill out your copyright notice in the Description page of Project Settings.


#include "ColorUtilityLibrary.h"

namespace
{
    static constexpr float HUE_RED = 0.0f;
    static constexpr float HUE_ORANGE = 30.0f;
    static constexpr float HUE_YELLOW = 60.0f;
    static constexpr float HUE_CHARTREUSE = 90.0f;
    static constexpr float HUE_GREEN = 120.0f;
    static constexpr float HUE_SPRING_GREEN = 150.0f;
    static constexpr float HUE_CYAN = 180.0f;
    static constexpr float HUE_AZURE = 210.0f;
    static constexpr float HUE_BLUE = 240.0f;
    static constexpr float HUE_VIOLET = 270.0f;
    static constexpr float HUE_MAGENTA = 300.0f;
    static constexpr float HUE_ROSE = 330.0f;
}
// =======================
// 色空間変換
// =======================

float UColorUtilityLibrary::GetHue(const FLinearColor& Color)
{
    // UEの組み込み関数を使用（HSVのR成分が色相）
    const FLinearColor HSV = Color.LinearRGBToHSV();
    return HSV.R;  // 0〜360度
}

FLinearColor UColorUtilityLibrary::FromHue(float HueDegrees)
{
    // 色相のみから色を生成（彩度=1.0, 明度=1.0）
    FLinearColor HSV;
    HSV.R = FMath::Fmod(HueDegrees, 360.0f);  // 色相（0〜360）
    HSV.G = 1.0f;  // 彩度（最大）
    HSV.B = 1.0f;  // 明度（最大）
    HSV.A = 1.0f;  // アルファ

    return HSV.HSVToLinearRGB();
}

// =======================
// 色相角度の差（距離）
// =======================

float UColorUtilityLibrary::GetHueAngleDistance(const FLinearColor& ColorA,
    const FLinearColor& ColorB)
{
    const float HueA = GetHue(ColorA);
    const float HueB = GetHue(ColorB);

    // 色相環での最短距離を計算
    const float Delta = FMath::Abs(HueA - HueB);
    return FMath::Min(Delta, 360.0f - Delta);
}

float UColorUtilityLibrary::GetHueDistanceFromAngle(const FLinearColor& Color,
    float ReferenceHue)
{
    const float Hue = GetHue(Color);

    // 基準角度からの最短距離
    const float Delta = FMath::Abs(Hue - ReferenceHue);
    return FMath::Min(Delta, 360.0f - Delta);
}


float UColorUtilityLibrary::GetColorRatio(const FLinearColor& ColorA, const FLinearColor& ColorB)
{
    float A, B = 0;
    A = GetHue(ColorA);
    B = GetHue(ColorB);

    // 360度の剰余を取りつつ差を計算
    float diff = FMath::Fmod(FMath::Abs(A - B) + 360.0f, 360.0f);
    diff = FMath::Min(diff, 360.0f - diff);
    float similarity = 1.0f - diff / 180.0f;
    return similarity;
}

float UColorUtilityLibrary::GetColorRatioWithTolerance(const FLinearColor& ColorA, const FLinearColor& ColorB, float Tolerance)
{
    float A = GetHue(ColorA);
    float B = GetHue(ColorB);
    float ClampedTolerance = FMath::Clamp(Tolerance, -180.0f, 180.0f);
    // 色相差の絶対値を0～180度の最小角度差で計算
    float diff = FMath::Fmod(FMath::Abs(A - B) + 360.0f, 360.0f);
    diff = FMath::Min(diff, 360.0f - diff);

    // diffが±ClampedToleranceの範囲に入っているかチェック
    if (diff > ClampedTolerance)
    {
        return 0.0f;  // 範囲外なら0
    }

    // 範囲内なら、diffが0で1、diffがClampedToleranceで0になるように正規化して返す
    float normalized = 1.0f - (diff / ClampedTolerance);
    return normalized;
}


// =======================
// 色相判定
// =======================

bool UColorUtilityLibrary::IsHueSimilar(const FLinearColor& ColorA,
    const FLinearColor& ColorB,
    float ThresholdDegrees)
{
    const float Distance = GetHueAngleDistance(ColorA, ColorB);
    return Distance <= ThresholdDegrees;
}

bool UColorUtilityLibrary::IsHueInRange(const FLinearColor& Color,
    float CenterHue,
    float RangeDegrees)
{
    const float Distance = GetHueDistanceFromAngle(Color, CenterHue);
    return Distance <= RangeDegrees;
}

// =======================
// 色相操作
// =======================

FLinearColor UColorUtilityLibrary::GetComplementaryColor(const FLinearColor& InColor)
{
    // 単純に色相を180度回転するだけ
    return RotateHue(InColor, 180.0f);
}

FLinearColor UColorUtilityLibrary::RotateHue(const FLinearColor& InColor,
    float RotationDegrees)
{
    // HSVに変換
    FLinearColor HSV = InColor.LinearRGBToHSV();

    // 色相を回転
    HSV.R = FMath::Fmod(HSV.R + RotationDegrees, 360.0f);
    if (HSV.R < 0.0f) HSV.R += 360.0f;

    // RGBに戻す
    return HSV.HSVToLinearRGB();
}

EColorCategory UColorUtilityLibrary::GetNearestPrimaryColor(const FLinearColor& Color)
{
    float Hue = GetHue(Color);

    // 各基本色との色相差を計算
    float DistanceRed = GetHueDistanceFromAngle(Color, HUE_RED);
    float DistanceGreen = GetHueDistanceFromAngle(Color, HUE_GREEN);
    float DistanceBlue = GetHueDistanceFromAngle(Color, HUE_BLUE);
    float DistanceYellow = GetHueDistanceFromAngle(Color, HUE_YELLOW);

    // 最小距離を探す
    float MinDistance = DistanceRed;
    EColorCategory Result = EColorCategory::Red;

    if (DistanceGreen < MinDistance)
    {
        MinDistance = DistanceGreen;
        Result = EColorCategory::Green;
    }
    if (DistanceBlue < MinDistance)
    {
        MinDistance = DistanceBlue;
        Result = EColorCategory::Blue;
    }
    if (DistanceYellow < MinDistance)
    {
        MinDistance = DistanceYellow;
        Result = EColorCategory::Yellow;
    }

    return Result;
}

// =======================
// エフェクト用ヘルパー
// =======================

FLinearColor UColorUtilityLibrary::EnhanceMaxComponent(const FLinearColor& Color,
    float Multiplier)
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
