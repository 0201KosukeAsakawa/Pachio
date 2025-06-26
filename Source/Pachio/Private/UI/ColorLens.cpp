#include "UI/ColorLens.h"
#include "Manager/ColorManager.h"
#include "Components/Image.h"
#include "Manager/LevelManager.h"

// UColorLens の初期化処理
void UColorLens::NativeConstruct()
{
    Super::NativeConstruct();

    // レベルマネージャのインスタンスを取得
    // GetWorld() から現在のワールドコンテキストを取得し、
    // シングルトン的に管理されている ALevelManager のインスタンスを取得する
    ALevelManager* Owner = ALevelManager::GetInstance(GetWorld());
    if (!Owner)
        return; // Ownerが取得できなければ初期化終了

    // ColorManager に対して、背景色変更の対象としてこのクラスを登録
    // 登録することで色変更イベントを受け取るようになる
    Owner->GetColorManager()->RegisterTarget(EColorTargetType::Responders, this);

}

void UColorLens::Animation(float i)
{
    //if (i < -1)
    //    i = -1;
    //else if (1 < i)
    //    i = 1;

    //if (RotationAnimation)
    //{
    //    PlayAnimation(RotationAnimation, 0.f, 1, EUMGSequencePlayMode::Forward,i);
    //}
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

void UColorLens::ColorAction(FLinearColor InColor)
{
    float H, S, V;
    ConvertRGBToHSV(InColor, H, S, V);

    if (FilterColorImage)
    {
        FWidgetTransform Transform = FilterColorImage->RenderTransform;
        Transform.Angle = H;  // 色相に応じて回転
        FilterColorImage->SetRenderTransform(Transform);
    }
}