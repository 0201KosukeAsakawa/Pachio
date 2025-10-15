// Fill out your copyright notice in the Description page of Project Settings.


#include "Logic/ColorManager/EffectColorRegistry.h"
#include "ColorUtilityLibrary.h"

UEffectColorRegistry::UEffectColorRegistry()
{
    InitializeDefaultColors();
}

// =======================
// 初期化
// =======================

void UEffectColorRegistry::InitializeDefaultColors()
{
    EffectColorMap = {
        { EColorCategory::Green,  FLinearColor(0.65f, 1.00f, 0.78f, 1.0f) }, // パステルミント
        { EColorCategory::Blue,   FLinearColor(0.65f, 0.78f, 1.00f, 1.0f) }, // パステルスカイブルー
        { EColorCategory::Red,    FLinearColor(1.00f, 0.75f, 0.65f, 1.0f) }, // パステルサーモン
        { EColorCategory::Yellow, FLinearColor(1.00f, 1.00f, 0.65f, 1.0f) }, // パステルイエロー
        { EColorCategory::Black,  FLinearColor(0.00f, 0.00f, 0.00f, 1.0f) }, // ブラック
    };
}

void UEffectColorRegistry::ResetToDefaultColors()
{
    InitializeDefaultColors();
}

// =======================
// エフェクト色の取得
// =======================

FLinearColor UEffectColorRegistry::GetEffectColor(EColorCategory Effect) const
{
    if (const FLinearColor* FoundColor = EffectColorMap.Find(Effect))
    {
        return *FoundColor;
    }

    // 見つからない場合はデフォルト値（白）を返す
    UE_LOG(LogTemp, Warning, TEXT("Effect color not found for effect type: %d"), static_cast<int32>(Effect));
    return FLinearColor::White;
}

// =======================
// 色登録・更新
// =======================

void UEffectColorRegistry::SetEffectColor(EColorCategory Effect, const FLinearColor& Color)
{
    EffectColorMap.Add(Effect, Color);
}