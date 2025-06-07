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

// 新しい色を受け取ってフィルターの色を更新する処理
void UColorLens::ColorAction(FLinearColor NewColor)
{
    if (!FilterColorImage)
        return; // Image コンポーネントが設定されていなければ処理しない

    // 現在のフィルター画像の色を取得
    FLinearColor CurrentColor = FilterColorImage->ColorAndOpacity;

    // 新しい色の RGB 成分を使い、Alpha（透明度）は現状のまま維持する
    FLinearColor CombinedColor = FLinearColor(
        NewColor.R,
        NewColor.G,
        NewColor.B,
        CurrentColor.A // 透明度は維持
    );

    // フィルター画像の色と透明度を更新
    FilterColorImage->SetColorAndOpacity(CombinedColor);
}
