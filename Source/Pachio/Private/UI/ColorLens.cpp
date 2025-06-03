// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ColorLens.h"
#include "Manager/ColorManager.h"
#include "Components/Image.h"
#include "Manager/LevelManager.h"

void UColorLens::NativeConstruct()
{
    Super::NativeConstruct();

    // Owner を探して ColorManager を取得（もしくはゲーム全体のシングルトンから取得でも可）
    ALevelManager* Owner = ALevelManager::GetInstance(GetWorld());
    if (!Owner) return;

    UColorManager* ColorManager = Owner->FindComponentByClass<UColorManager>();
    if (ColorManager)
    {
        ColorManager->RegisterTarget(EColorMode::Background, this);  // 例えば背景色変更モードに登録
    }
}


void UColorLens::SetColor(FLinearColor NewColor)
{
    if (!FilterColorImage)
        return;

    // 現在の色を取得
    FLinearColor CurrentColor = FilterColorImage->ColorAndOpacity;

    // RGB は新しい色に更新し、Alpha は既存値を維持
    FLinearColor CombinedColor = FLinearColor(
        NewColor.R,
        NewColor.G,
        NewColor.B,
        CurrentColor.A // Alphaだけ元のまま
    );

    FilterColorImage->SetColorAndOpacity(CombinedColor);
}
