// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/ColorControllerComponent.h"
#include "FunctionLibrary.h"

// Sets default values for this component's properties
UColorControllerComponent::UColorControllerComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UColorControllerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{

}

void UColorControllerComponent::AdjustColor(EColorChannel Channel, float Delta)
{
    // RGB → HSV に変換
    FLinearColor HSV = CurrentColor.LinearRGBToHSV();

    float Hue = HSV.R;  // 0〜360
    float Saturation = HSV.G;
    float Value = HSV.B;

    // 彩度か明度がほぼ0なら少し上げて色相の変化が見えるようにする
    if (Saturation < KINDA_SMALL_NUMBER)
    {
        Saturation = 1.f;
    }
    if (Value < KINDA_SMALL_NUMBER)
    {
        Value = 1.f;
    }

    // Hue を調整
    Hue += Delta * 360.0f;

    if (Hue > 360.f)
        Hue -= 360.f;
    if (Hue < 0.f)
        Hue += 360.f;

    // HSV → RGB に変換
    FLinearColor NewColor = FLinearColor(Hue, Saturation, Value).HSVToLinearRGB();

    // 現在の色を更新（アルファも保持）
    CurrentColor.R = NewColor.R;
    CurrentColor.G = NewColor.G;
    CurrentColor.B = NewColor.B;

    UE_LOG(LogTemp, Log, TEXT("End Color changed:RGB: R=%.3f G=%.3f B=%.3f"), NewColor.R, NewColor.G, NewColor.B);

    // デリゲートを通知
    OnColorChanged.Broadcast(CurrentColor ,Mode);
}

void UColorControllerComponent::ChangeMode(int Direction)
{
    // Direction が正のときは次、負のときは前
    if (Direction > 0)
    {
        Mode = GetNextMode(Mode);
    }
    else if (Direction < 0)
    {
        Mode = GetPreviousMode(Mode);
    }

    // モードを表示（デバッグ用）
    UE_LOG(LogTemp, Warning, TEXT("New Mode: %d"), static_cast<int32>(Mode));

}

EColorTargetType UColorControllerComponent::GetNextMode(EColorTargetType CurrentMode)
{
    // EColorTargetTypeの範囲を取得
    const TArray<EColorTargetType> AllModes = UFunctionLibrary::GetAllEnumValues<EColorTargetType>();

    // 現在のモードが最後の要素なら最初に戻る
    int32 CurrentIndex = AllModes.IndexOfByKey(CurrentMode);
    if (CurrentIndex == INDEX_NONE)
    {
        return EColorTargetType::Layer; // デフォルトのモードに戻す
    }

    // 次のモードに進む。最後の要素の場合、最初に戻る。
    return AllModes[(CurrentIndex + 1) % AllModes.Num()];
}

EColorTargetType UColorControllerComponent::GetPreviousMode(EColorTargetType CurrentMode)
{
    // EColorTargetTypeの範囲を取得
    const TArray<EColorTargetType> AllModes = UFunctionLibrary::GetAllEnumValues<EColorTargetType>();

    // 現在のモードが最初の要素なら最後に戻る
    int32 CurrentIndex = AllModes.IndexOfByKey(CurrentMode);
    if (CurrentIndex == INDEX_NONE)
    {
        return EColorTargetType::Layer; // デフォルトのモードに戻す
    }

    // 前のモードに戻る。最初の要素の場合、最後に戻る。
    return AllModes[(CurrentIndex - 1 + AllModes.Num()) % AllModes.Num()];
}

