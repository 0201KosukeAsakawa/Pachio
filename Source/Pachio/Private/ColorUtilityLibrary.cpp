// Fill out your copyright notice in the Description page of Project Settings.


#include "ColorUtilityLibrary.h"

namespace
{
    // FVector(X: Hue, Y: Saturation, Z: Lightness)
    static const FVector HSL_RED = FVector(0.0f, 1.0f, 1.0f);
    static const FVector HSL_ORANGE = FVector(30.0f, 1.0f, 1.0f);
    static const FVector HSL_YELLOW = FVector(60.0f, 1.0f, 1.0f);
    static const FVector HSL_CHARTREUSE = FVector(90.0f, 1.0f, 1.0f);
    static const FVector HSL_GREEN = FVector(120.0f, 1.0f, 1.0f);
    static const FVector HSL_SPRING_GREEN = FVector(150.0f, 1.0f, 1.0f);
    static const FVector HSL_CYAN = FVector(180.0f, 1.0f, 1.0f);
    static const FVector HSL_AZURE = FVector(210.0f, 1.0f, 1.0f);
    static const FVector HSL_BLUE = FVector(240.0f, 1.0f, 1.0f);
    static const FVector HSL_VIOLET = FVector(270.0f, 1.0f, 1.0f);
    static const FVector HSL_MAGENTA = FVector(300.0f, 1.0f, 1.0f);
    static const FVector HSL_ROSE = FVector(330.0f, 1.0f, 1.0f);
}

TMap<EColorCategory, FLinearColor> UColorUtilityLibrary::EffectColorMap = {
    { EColorCategory::Red,          UColorUtilityLibrary::FromHSL(HSL_RED) },
    { EColorCategory::Orange,       UColorUtilityLibrary::FromHSL(HSL_ORANGE) },
    { EColorCategory::Yellow,       UColorUtilityLibrary::FromHSL(HSL_YELLOW) },
    { EColorCategory::Chatreuse,    UColorUtilityLibrary::FromHSL(HSL_CHARTREUSE) },
    { EColorCategory::Green,        UColorUtilityLibrary::FromHSL(HSL_GREEN) },
    { EColorCategory::SpringGreen,  UColorUtilityLibrary::FromHSL(HSL_SPRING_GREEN) },
    { EColorCategory::Cyan,         UColorUtilityLibrary::FromHSL(HSL_CYAN) },
    { EColorCategory::Azure,        UColorUtilityLibrary::FromHSL(HSL_AZURE) },
    { EColorCategory::Blue,         UColorUtilityLibrary::FromHSL(HSL_BLUE) },
    { EColorCategory::Violet,       UColorUtilityLibrary::FromHSL(HSL_VIOLET) },
    { EColorCategory::Magenta,      UColorUtilityLibrary::FromHSL(HSL_MAGENTA) },
    { EColorCategory::Rose,         UColorUtilityLibrary::FromHSL(HSL_ROSE) },
    { EColorCategory::Black,        FLinearColor::Black },
    { EColorCategory::White,        FLinearColor::White }
};


// =======================
// 色空間変換
// =======================

FVector UColorUtilityLibrary::GetHSL(const FLinearColor& Color)
{
    // UEの組み込み関数を使用（HSVのR成分が色相）
    const FLinearColor HSV = Color.LinearRGBToHSV();
    return FVector(HSV.R, HSV.G, HSV.B);  // 0〜360度
}

FLinearColor UColorUtilityLibrary::FromHSL(FVector HueDegrees)
{
    // 色相のみから色を生成（彩度=1.0, 明度=1.0）
    FLinearColor HSV;
    HSV.R = FMath::Fmod(HueDegrees.X, 360.0f);  // 色相（0〜360）
    HSV.G = FMath::Clamp(HueDegrees.Y, 0, 1);   // 彩度（0～１）
    HSV.B = FMath::Clamp(HueDegrees.Z, 0, 1);   // 明度（0～１）
    HSV.A = 1.0f;  // アルファ

    return HSV.HSVToLinearRGB();
}

// =======================
// 色相角度の差（距離）
// =======================

FVector UColorUtilityLibrary::GetHSLDistance(const FLinearColor& ColorA,
    const FLinearColor& ColorB)
{
    const FVector HSL_A = GetHSL(ColorA);
    const FVector HSL_B = GetHSL(ColorB);
    // 無彩色判定
    if (HSL_A.Y < 0.01f || HSL_B.Y < 0.01f)
    {
        // 無彩色同士なら Lightness の差だけ比較、Hueは無視
        return FVector(0.0f, 0.0f, FMath::Abs(HSL_A.Z - HSL_B.Z));
    }
    // 色相環での最短距離を計算
    const float Delta = FMath::Abs(HSL_A.X - HSL_B.X);
    const float Saturation = FMath::Abs(HSL_A.Y - HSL_B.Y);
    const float Lightness = FMath::Abs(HSL_A.Z - HSL_B.Z);
   
    return  FVector(FMath::Min(Delta, 360.0f - Delta), Saturation, Lightness);
}

float UColorUtilityLibrary::GetHueDistanceFromAngle(const FLinearColor& Color,
    float ReferenceHue)
{
    const FVector Hue = GetHSL(Color);

    // 基準角度からの最短距離
    const float Delta = (float)FMath::Abs(Hue.X - ReferenceHue);
    return FMath::Min(Delta, 360.0f - Delta);
}


float UColorUtilityLibrary::GetColorRatio(const FLinearColor& ColorA, const FLinearColor& ColorB)
{
    FVector A, B = FVector::ZeroVector;
    A = GetHSL(ColorA);
    B = GetHSL(ColorB);

    if (A == B)
        return 0;

    // 360度の剰余を取りつつ差を計算
    float diff = FMath::Fmod(FMath::Abs(A.X - B.X) + 360.0f, 360.0f);
    diff = FMath::Min(diff, 360.0f - diff);
    float similarity = 1.0f - diff / 180.0f;
    return similarity;
}

float UColorUtilityLibrary::GetColorRatioWithTolerance(const FLinearColor& ColorA, const FLinearColor& ColorB, float Tolerance)
{
    FVector A = GetHSL(ColorA);
    FVector B = GetHSL(ColorB);
    float ClampedTolerance = FMath::Clamp(Tolerance, -180.0f, 180.0f);
    // 色相差の絶対値を0～180度の最小角度差で計算
    float diff = FMath::Fmod(FMath::Abs(A.X - B.X) + 360.0f, 360.0f);
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
    FVector ThresholdHSL)
{
    if (ColorA == FLinearColor::White)
        return false;
    const FVector Distance = GetHSLDistance(ColorA, ColorB);
    return Distance.X <= ThresholdHSL.X /*&& Distance.Y <= ThresholdHSL.Y && Distance.Z <= ThresholdHSL.Z*/;
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

EColorCategory UColorUtilityLibrary::GetNearestColorCategory(const FLinearColor& Color)
{
    // 明度が低い場合など、黒扱い
    if (Color.R < 0.05f && Color.G < 0.05f && Color.B < 0.05f)
    {
        return EColorCategory::Black;
    }
    if (Color == FLinearColor::White)
        return EColorCategory::White;

    float MinDistance = FLT_MAX;
    EColorCategory Result = EColorCategory::Red;

    for (auto& Pair : EffectColorMap) // Pair.Key = Category, Pair.Value = FLinearColor
    {
        if (Pair.Key == EColorCategory::Black) continue; // 黒は除外

        float Distance = GetColorRatio(Color, Pair.Value); // EffectColorMap の色を使用
        if (Distance < MinDistance)
        {
            MinDistance = Distance;
            Result = Pair.Key;
        }
    }

    return Result;
}

FLinearColor UColorUtilityLibrary::BlendHSL(
    const FLinearColor& ColorA,
    const FLinearColor& ColorB,
    float MixingRatio /* 0.0 ～ 1.0 */
)
{
    FVector A = GetHSL(ColorA);
    FVector B = GetHSL(ColorB);

    // --- Hue（角度補間） ---
    float Delta = B.X - A.X;
    if (FMath::Abs(Delta) > 180.0f)
    {
        Delta -= FMath::Sign(Delta) * 360.0f;
    }

    float NewHue = FMath::Fmod(A.X + Delta * MixingRatio, 360.0f);
    if (NewHue < 0.0f)
    {
        NewHue += 360.0f;
    }

    // --- Saturation / Lightness ---
    float NewS = FMath::Lerp(A.Y, B.Y, MixingRatio);
    float NewL = FMath::Lerp(A.Z, B.Z, MixingRatio);

    return FromHSL(FVector(NewHue, NewS, NewL));
}

FLinearColor UColorUtilityLibrary::LerpHue(const FLinearColor& FromColor, const FLinearColor& ToColor, float Alpha)
{
    FVector H1 = GetHSL(FromColor);
    FVector H2 = GetHSL(ToColor);

    float Delta = H2.X - H1.X;
    if (Delta > 180.0f) Delta -= 360.0f;
    if (Delta < -180.0f) Delta += 360.0f;

    float H = H1.X + Delta * Alpha;
    H = FMath::Fmod(H, 360.0f);
    if (H < 0) H += 360.0f;

    constexpr float PastelS = 0.35f;   
    constexpr float PastelV = 0.95f;   

    return FLinearColor::MakeFromHSV8(
        (uint8)(H / 360.0f * 255.0f),
        (uint8)(PastelS * 255.0f),
        (uint8)(PastelV * 255.0f)
    );
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

EColorCategory UColorUtilityLibrary::GetNearestColorCategoryRGBY(const FLinearColor& Color)
{
    // 明度が低い場合など、黒扱い
    if (Color.R < 0.05f && Color.G < 0.05f && Color.B < 0.05f)
    {
        return EColorCategory::Black;
    }

    if (Color == FLinearColor::White)
        return EColorCategory::White;

     float MinDistance = FLT_MAX;
    EColorCategory Result = EColorCategory::Red;

    for (auto& Pair : EffectColorMap) // Pair.Key = Category, Pair.Value = FLinearColor
    {
        if (Pair.Key != EColorCategory::Red && Pair.Key != EColorCategory::Blue
            &&  Pair.Key != EColorCategory::Green && Pair.Key != EColorCategory::White)
            continue; 

        float Distance = GetColorRatio(Color, Pair.Value); // EffectColorMap の色を使用
        if (Distance < MinDistance)
        {
            MinDistance = Distance;
            Result = Pair.Key;
        }
    }

    return Result;
}

FLinearColor UColorUtilityLibrary::GetCategoryColor(EColorCategory targetCategory)
{
    if (const FLinearColor* FoundColor = EffectColorMap.Find(targetCategory))
    {
        return *FoundColor;
    }

    // 見つからない場合はデフォルト値（白）を返す
    UE_LOG(LogTemp, Warning, TEXT("Effect color not found for effect type: %d"), static_cast<int32>(targetCategory));
    return FLinearColor::White;
}
