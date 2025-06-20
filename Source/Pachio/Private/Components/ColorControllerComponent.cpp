// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/ColorControllerComponent.h"
#include "DataContainer/EffectMatchResult.h"
#include "FunctionLibrary.h"
#include "UI/ColorLens.h"

// Sets default values for this component's properties
UColorControllerComponent::UColorControllerComponent()
{
    PrimaryComponentTick.bCanEverTick = true;

    // カラーマップを EColorTargetType ごとに白で初期化
    ColorMap.Empty(); // 念のため初期化（既存がある場合）

    const TArray<EColorTargetType> AllModes = UFunctionLibrary::GetAllEnumValues<EColorTargetType>();
    for (EColorTargetType Mode : AllModes)
    {
        if (Mode == EColorTargetType::Responders || Mode == EColorTargetType::Event)
            continue;

        ColorMap.Add(Mode, FLinearColor::White);
    }
}

void UColorControllerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{

}

void UColorControllerComponent::AdjustColor(float Delta)
{
    // RGB → HSV に変換
    FLinearColor HSV = ColorMap[CurrentColorMode].LinearRGBToHSV();

    float Hue = HSV.R;  // 0〜360
    float Saturation = HSV.G;
    float Value = HSV.B;

    // パステルカラー用に彩度と明度を制限
    Saturation = FMath::Clamp(Saturation, 0.2f, 0.6f);
    Value = FMath::Clamp(Value, 0.8f, 1.0f);

    // Hue を調整
    Hue += Delta * 360.0f;
    Hue = FMath::Fmod(Hue, 360.0f);
    if (Hue < 0.f)
        Hue += 360.f;

    // HSV → RGB に変換
    FLinearColor NewColor = FLinearColor(Hue, Saturation, Value).HSVToLinearRGB();

    // 現在の色を更新（アルファも保持）
    ColorMap[CurrentColorMode].R = NewColor.R;
    ColorMap[CurrentColorMode].G = NewColor.G;
    ColorMap[CurrentColorMode].B = NewColor.B;

    // デリゲートを通知
    OnColorChanged.Broadcast(ColorMap[CurrentColorMode], CurrentColorMode);
}


void UColorControllerComponent::ChangeMode(int Direction)
{
    // Directionが1以上なら1、それ未満なら-1に補正
    if (Direction >= 1)
    {
        Direction = 1;
    }
    else
    {
        Direction = -1;
    }

    // Direction が正のときは次、負のときは前
    if (Direction > 0)
    {
        CurrentColorMode = GetNextMode(CurrentColorMode);
    }
    else if (Direction < 0)
    {
        CurrentColorMode = GetPreviousMode(CurrentColorMode);
    }
    // モードを表示（デバッグ用）
    UE_LOG(LogTemp, Warning, TEXT("New Mode: %d"), static_cast<int32>(CurrentColorMode));
    AnimationDelegate.Execute(Direction);
}

EColorTargetType UColorControllerComponent::GetNextMode(EColorTargetType CurrentMode)
{
    // Responders を除外したリストを取得
    TArray<EColorTargetType> FilteredModes;
    const TArray<EColorTargetType> AllModes = UFunctionLibrary::GetAllEnumValues<EColorTargetType>();

    for (EColorTargetType Mode : AllModes)
    {
        if (Mode == EColorTargetType::Responders)
            continue;

        if (Mode == EColorTargetType::Event)
            continue;

        FilteredModes.Add(Mode);

    }

    int32 CurrentIndex = FilteredModes.IndexOfByKey(CurrentMode);
    if (CurrentIndex == INDEX_NONE)
    {
        return EColorTargetType::Layer; // デフォルトに戻す
    }

    return FilteredModes[(CurrentIndex + 1) % FilteredModes.Num()];
}

EColorTargetType UColorControllerComponent::GetPreviousMode(EColorTargetType CurrentMode)
{
    // Responders を除外したリストを取得
    TArray<EColorTargetType> FilteredModes;
    const TArray<EColorTargetType> AllModes = UFunctionLibrary::GetAllEnumValues<EColorTargetType>();

    for (EColorTargetType Mode : AllModes)
    {
        if (Mode == EColorTargetType::Responders)
            continue;

        if (Mode == EColorTargetType::Event)
            continue;

        FilteredModes.Add(Mode);

    }

    int32 CurrentIndex = FilteredModes.IndexOfByKey(CurrentMode);
    if (CurrentIndex == INDEX_NONE)
    {
        return EColorTargetType::Layer; // デフォルトに戻す
    }

    return FilteredModes[(CurrentIndex - 1 + FilteredModes.Num()) % FilteredModes.Num()];
}
