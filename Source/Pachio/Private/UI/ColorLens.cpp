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
    Owner->GetColorManager()->RegisterTarget(EColorTargetType::ObjectTypeGamma, this);

}

void UColorLens::Animation(float i)
{
    if (i < -1)
        i = -1;
    else if (1 < i)
        i = 1;

    if (RotationAnimation)
    {
        PlayAnimation(RotationAnimation, 0.f, 1, EUMGSequencePlayMode::Forward,i);
    }
}

void UColorLens::ColorAction(FLinearColor newColor)
{


    if (FilterColorImage)
    {
        FilterColorImage->SetColorAndOpacity(newColor);
    }
}