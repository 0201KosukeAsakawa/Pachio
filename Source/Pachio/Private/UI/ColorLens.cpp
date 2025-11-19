#include "UI/ColorLens.h"
#include "Components/Image.h"
#include "Manager/ColorManager.h"
#include "Sound/SoundManager.h"
#include "Manager/LevelManager.h"

// UColorLens の初期化処理
void UColorLens::NativeConstruct()
{
    Super::NativeConstruct();
    if (FilterColorImage)
    {
        OriginalScale = FilterColorImage->RenderTransform.Scale;
    }

    // レベルマネージャのインスタンスを取得
    // GetWorld() から現在のワールドコンテキストを取得し、
    // シングルトン的に管理されている ALevelManager のインスタンスを取得する
    ALevelManager* Owner = ALevelManager::GetInstance(GetWorld());
    if (!Owner)
        return; // Ownerが取得できなければ初期化終了

    // ColorManager に対して、背景色変更の対象としてこのクラスを登録
    // 登録することで色変更イベントを受け取るようになる
    Owner->GetColorManager()->RegisterTarget(this);
}

void UColorLens::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    if (!bIsAnimating || !FilterColorImage) return;

    AnimationTime += InDeltaTime;
    float HalfDuration = AnimationDuration * 0.5f;

    float ScaleFactor = 1.f;

    if (AnimationTime <= HalfDuration)
    {
        // 拡大フェーズ（0～半分）
        ScaleFactor = FMath::Lerp(OriginalScale.X, TargetScale.X, AnimationTime / HalfDuration);
    }
    else if (AnimationTime <= AnimationDuration)
    {
        // 縮小フェーズ（半分～終わり）
        ScaleFactor = FMath::Lerp(TargetScale.X, OriginalScale.X, (AnimationTime - HalfDuration) / HalfDuration);
    }
    else
    {
        // アニメーション終了
        ScaleFactor = OriginalScale.X;
        bIsAnimating = false;
    }

    FWidgetTransform FilterColorTransform = FilterColorImage->RenderTransform;
    FilterColorTransform.Scale = FVector2D(ScaleFactor, ScaleFactor);
    FilterColorImage->SetRenderTransform(FilterColorTransform);

    FWidgetTransform ColorCircleTransform = ColorCircle->RenderTransform;
    ColorCircleTransform.Scale = FVector2D(ScaleFactor, -ScaleFactor);
    ColorCircle->SetRenderTransform(ColorCircleTransform);
}

void UColorLens::Animation(float i)
{

}

void UColorLens::PlayBeatAnimation()
{
    if (bIsAnimating) return;  // 連打防止（任意）

    AnimationTime = 0.f;
    bIsAnimating = true;
}

void ConvertRGBToHSV(const FLinearColor& InColor, float& OutH, float& OutS, float& OutV)
{
    float R = InColor.R;
    float G = InColor.G;
    float B = InColor.B;

    float Max = FMath::Max3(R, G, B);
    float Min = FMath::Min3(R, G, B);
    float Delta = Max - Min;

    // Hue
    if (Delta == 0)
    {
        OutH = 0;
    }
    else if (Max == R)
    {
        OutH = 60.f * FMath::Fmod(((G - B) / Delta), 6.f);
    }
    else if (Max == G)
    {
        OutH = 60.f * (((B - R) / Delta) + 2.f);
    }
    else // Max == B
    {
        OutH = 60.f * (((R - G) / Delta) + 4.f);
    }

    if (OutH < 0)
        OutH += 360.f;

    // Saturation
    OutS = (Max == 0.f) ? 0.f : Delta / Max;

    // Value
    OutV = Max;
}

FLinearColor AdjustColor(FLinearColor InColor)
{
    // RGB → HSV に変換
    FLinearColor HSV = InColor.LinearRGBToHSV();

    // 彩度（S）と明度（V）を補正
    HSV.G = FMath::Clamp(HSV.G * 1.5f, 0.0f, 1.0f); // 彩度を強調
    HSV.B = FMath::Clamp(HSV.B * 0.7f, 0.0f, 1.0f); // 明度を下げて濃く

    // HSV → RGB に戻して返す
    return HSV.HSVToLinearRGB();
}

void UColorLens::ApplyColorWithMatching(const FLinearColor& NewColor)
{
    if (FilterColorImage == nullptr)
        return;

    float H, S, V;
    ConvertRGBToHSV(NewColor, H, S, V);

    // メイン画像の色設定
    FilterColorImage->SetColorAndOpacity(AdjustColor(NewColor));


    // H を角度に変換（0~360）
    float AngleDeg = H - 90.0f;
    float AngleRad = FMath::DegreesToRadians(AngleDeg);

    // 半径（中心からの距離）
    float Radius = 150.0f; // 好きな距離に調整

    // 中心からのオフセット計算（X=cos, Y=sin）
    FVector2D Offset(
        FMath::Cos(AngleRad) * Radius,
        FMath::Sin(AngleRad) * Radius
    );

    // 画像のTransformを更新
    FWidgetTransform Transform;
    Transform.Translation = Offset;
    FilterColorImage->SetRenderTransform(Transform);

}
