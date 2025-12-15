// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/Color/ColorControllerComponent.h"
#include "Components/Color/ObjectColorComponent.h"

#include "DataContainer/ColorTargetTypes.h"

#include "FunctionLibrary.h"
#include "ColorUtilityLibrary.h"

#include "UI/ColorLens.h"
#include "UI/UIManager.h"
#include "Manager/LevelManager.h"
#include "Manager/ColorManager.h"

#include "Kismet/KismetSystemLibrary.h" 


// =======================
// コンストラクタ
// =======================

// Sets default values for this component's properties
UColorControllerComponent::UColorControllerComponent()
{
    PrimaryComponentTick.bCanEverTick = true;

    // 初期色は白
    CurrentColor = FLinearColor::White;

    ColorTankMap.Add(EColorCategory::Red, 0);
    //ColorTankMap.Add(EColorCategory::Orange, 0);
    //ColorTankMap.Add(EColorCategory::Yellow, 0);
    //ColorTankMap.Add(EColorCategory::Chatreuse, 0);
    ColorTankMap.Add(EColorCategory::Green, 0);
    //ColorTankMap.Add(EColorCategory::SpringGreen, 0);
    //ColorTankMap.Add(EColorCategory::Cyan, 0);
    //ColorTankMap.Add(EColorCategory::Azure, 0);
    ColorTankMap.Add(EColorCategory::Blue, 0);
    //ColorTankMap.Add(EColorCategory::Violet, 0);
    //ColorTankMap.Add(EColorCategory::Magenta, 0);
    //ColorTankMap.Add(EColorCategory::Rose, 0);
    ColorTankMap.Add(EColorCategory::White, 0);
}

// =======================
// Tick（フレーム更新処理）
// =======================

void UColorControllerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    UObjectColorComponent* TargetComp = GetHitColorComponent(1500.f);
    if (!TargetComp) return;

    if (bPlayPaint)
    {
        PaintHitObject(TargetComp);
    }
    else
    {
        AbsorbHitObject(TargetComp);
    }
}

// =======================
// 色調整系
// =======================


//void UColorControllerComponent::AdjustColor(float Delta)
//{
//    // 現在モードの色を HSV に変換
//    FLinearColor HSV = CurrentColor.LinearRGBToHSV();
//
//    float Hue = HSV.R;                                  // 色相
//    float Saturation = FMath::Clamp(HSV.G, 0.1f, 0.3f); // 彩度（固定範囲に制限）
//    float Value = FMath::Clamp(HSV.B, 0.8f, 1.0f);      // 明度（明るめを維持）
//
//    // Hue を Delta 分だけ回転
//    Hue = FMath::Fmod(Hue + Delta * 360.0f, 360.0f);
//    if (Hue < 0.f) 
//        Hue += 360.f;
//
//    UE_LOG(LogTemp, Log, TEXT("Hue : : %f"), Hue);
//    // HSV → RGB に戻す
//    FLinearColor NewColor = FLinearColor(Hue, Saturation, Value).HSVToLinearRGB();
//
//    // 現在モードの色を更新（αは保持）
//    CurrentColor = FLinearColor(NewColor.R, NewColor.G, NewColor.B, CurrentColor.A);
//    // イベントを通知
//    //OnColorChanged.Broadcast(CurrentColor);
//}


void UColorControllerComponent::AdjustColor(float Delta)
{
    // 現在モードの色を HSV に変換
    FLinearColor HSV = CurrentColor.LinearRGBToHSV();
    float Hue = HSV.R; // 色相d

    // Hue を Delta 分だけ回転
    Hue = FMath::Fmod(Hue + Delta, 360.0f);
    if (Hue < 0.f)
        Hue += 360.f;

    UE_LOG(LogTemp, Log, TEXT("Hue : : %f"), Hue);

    // ColorTankMapから有効な色カテゴリを取得（値が1以上のもの）
    TArray<EColorCategory> ValidCategories;
    for (const auto& Pair : ColorTankMap)
    {
        if (Pair.Value > 0)
        {
            ValidCategories.Add(Pair.Key);
        }
    }

    // 有効なカテゴリがない場合は処理を中断
    if (ValidCategories.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("No valid color categories available"));
        return;
    }

    // 色相環を等分割（360度 / カテゴリ数）
    float AnglePerCategory = 360.0f / ValidCategories.Num();

    // 現在の角度がどの区分に属するかを判定
    int32 CategoryIndex = FMath::FloorToInt(Hue / AnglePerCategory) % ValidCategories.Num();

    // 判定されたカテゴリを取得
    EColorCategory TargetCategory = ValidCategories[CategoryIndex];

    UE_LOG(LogTemp, Log, TEXT("Selected Category Index: %d, AnglePerCategory: %f"),
        CategoryIndex, AnglePerCategory);

    // カテゴリから色を取得
    FLinearColor NewColor = UColorUtilityLibrary::GetCategoryColor(TargetCategory);

    // CurrentColorを更新（αは保持）
    CurrentColor = FLinearColor(NewColor.R, NewColor.G, NewColor.B, CurrentColor.A);

    // イベントを通知
    // OnColorChanged.Broadcast(CurrentColor);
}

FLinearColor UColorControllerComponent::GetCurrentColor() const
{
    return CurrentColor;
}


UObjectColorComponent* UColorControllerComponent::GetHitColorComponent(float Distance)
{
    FVector Start = GetOwner()->GetActorLocation();
    FVector Direction = GetOwner()->GetActorRightVector().GetSafeNormal();
    FVector End = Start + Direction * Distance;

    FHitResult HitResult;
    TArray<AActor*> ActorsToIgnore;
    ActorsToIgnore.Add(GetOwner());

    bool bHit = UKismetSystemLibrary::LineTraceSingle(
        this,
        Start,
        End,
        UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_WorldStatic),
        false,
        ActorsToIgnore,
        EDrawDebugTrace::ForDuration,
        HitResult,
        true,
        CurrentColor,
        CurrentColor,
        0.5f
    );

    if (bHit && HitResult.GetActor())
    {
        return HitResult.GetActor()->GetComponentByClass<UObjectColorComponent>();
    }

    return nullptr;
}
void UColorControllerComponent::PaintHitObject(UObjectColorComponent* TargetComp)
{
    if (!TargetComp) return;

    FLinearColor TraceColor = UColorUtilityLibrary::GetCategoryColor(
        UColorUtilityLibrary::GetNearestColorCategoryRGBY(CurrentColor)
    );

    TargetComp->ApplyColorWithMatching(TraceColor);
}

void UColorControllerComponent::AbsorbHitObject(UObjectColorComponent* TargetComp)
{
    if (!TargetComp) return;

    // ヒットした色を取得
    FLinearColor HitColor = TargetComp->GetCurrentColor();

    // HSL変換
    FVector HSL = UColorUtilityLibrary::GetHSL(HitColor);
    float Saturation = HSL.Y;
    float Lightness = HSL.Z;

    // 無彩色（ほぼ白や灰色）は White に加算
    if (Saturation < 0.01f)
    {
        ColorTankMap.FindOrAdd(EColorCategory::White) += 1;
        return;
    }

    // RGB比率に応じて主要色を抽出
    TArray<TPair<EColorCategory, float>> Components;

    // 赤・緑・青のみTankに加算
    if (HitColor.R > 0.5f) Components.Add({ EColorCategory::Red, HitColor.R });
    if (HitColor.G > 0.5f) Components.Add({ EColorCategory::Green, HitColor.G });
    if (HitColor.B > 0.5f) Components.Add({ EColorCategory::Blue, HitColor.B });

    // 明るい色や薄い色は White も加算
    if (Lightness > 0.8f || HSL.Y < 0.2f)
    {
        Components.Add({ EColorCategory::White, 0.5f }); // 半分くらいをWhiteに分配
    }

    // Tankに加算（正規化して整数化も可）
    for (auto& Comp : Components)
    {
        int32 AddAmount = FMath::RoundToInt(Comp.Value * 1); // 1にスケール調整可
        ColorTankMap.FindOrAdd(Comp.Key) += AddAmount;
    }

    TargetComp->ApplyColorWithMatching(FLinearColor::White);
}
