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

namespace
{
    constexpr int32 ABSORB_AMOUNT = 5;      // 吸収時の回収量
    constexpr int32 MAX_TANK_CAPACITY = 10;  // タンクの最大容量
    constexpr int32 MIN_TANK_CAPACITY = 0;   // タンクの最小容量
}

// =======================
// コンストラクタ
// =======================

// Sets default values for this component's properties
UColorControllerComponent::UColorControllerComponent()
{
    PrimaryComponentTick.bCanEverTick = true;

    // 初期色は白
    CurrentColor = FLinearColor::White;

    ColorTankMap.Add(EColorCategory::Red, 10);
    ColorTankMap.Add(EColorCategory::Green, 10);
    ColorTankMap.Add(EColorCategory::Blue, 10);
    ColorTankMap.Add(EColorCategory::White, 10);
}

// =======================
// Tick（フレーム更新処理）
// =======================

void UColorControllerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
 
}

// =======================
// 色調整系
// =======================
void UColorControllerComponent::AdjustColor(float Delta)
{
    // 現在モードの色を HSV に変換
    FLinearColor HSV = CurrentColor.LinearRGBToHSV();
    float Hue = HSV.R; // 色相d

    // Hue を Delta 分だけ回転
    Hue = FMath::Fmod(Hue + Delta, 360.0f);
    if (Hue < 0.f)
        Hue += 360.f;

    //UE_LOG(LogTemp, Log, TEXT("Hue : : %f"), Hue);

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
        //UE_LOG(LogTemp, Warning, TEXT("No valid color categories available"));
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

    // Box の半径（必要に応じて調整）
    FVector BoxExtent = FVector(50.f, 50.f, 50.f);

    // Box の向き（Actorの向きに合わせる）
    FRotator BoxRotation = GetOwner()->GetActorRotation();

    FHitResult HitResult;
    TArray<AActor*> ActorsToIgnore;
    ActorsToIgnore.Add(GetOwner());

    bool bHit = UKismetSystemLibrary::BoxTraceSingle(
        this,
        Start,
        End,
        BoxExtent,
        BoxRotation,
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

    // 対象が変化中なら吸収できない
    if (TargetComp->IsPainting())
    {
        UE_LOG(LogTemp, Warning, TEXT("Target is currently painting, cannot absorb"));
        return;
    }

    // ヒットした色を取得
    FLinearColor HitColor = TargetComp->GetCurrentColor();

    // HSL変換
    FVector HSL = UColorUtilityLibrary::GetHSL(HitColor);
    float Saturation = HSL.Y;
    float Lightness = HSL.Z;

    // 無彩色（ほぼ白や灰色）は White に加算
    if (Saturation < 0.01f)
    {
        int32& WhiteTank = ColorTankMap.FindOrAdd(EColorCategory::White);
        WhiteTank = FMath::Min(WhiteTank + ABSORB_AMOUNT, MAX_TANK_CAPACITY);

        TargetComp->SetTargetColor(FLinearColor::White);
        UE_LOG(LogTemp, Log, TEXT("Absorbed White: %d"), WhiteTank);
        return;
    }

    // RGB比率に応じて主要色を抽出
    TArray<TPair<EColorCategory, float>> Components;

    // 赤・緑・青のみTankに加算
    if (HitColor.R > 0.7f) 
    {
        Components.Add({ EColorCategory::Red, HitColor.R });
        UE_LOG(LogTemp, Log, TEXT("Copy_Red"));
    }
    if (HitColor.G > 0.7f) 
    {
        Components.Add({ EColorCategory::Green, HitColor.G });
        UE_LOG(LogTemp, Log, TEXT("Copy_Green"));
    }
    if (HitColor.B > 0.7f) 
    {
        Components.Add({ EColorCategory::Blue, HitColor.B });
        UE_LOG(LogTemp, Log, TEXT("Copy_Blue"));
    }

    // 明るい色や薄い色は White も加算
    if (Lightness > 0.8f || HSL.Y < 0.2f)
    {
        Components.Add({ EColorCategory::White, 0.5f });
    }

    // 合計の重みを計算
    float TotalWeight = 0.0f;
    for (const auto& Comp : Components)
    {
        TotalWeight += Comp.Value;
    }

    // Tankに加算（容量制限あり）
    if (TotalWeight > 0.0f)
    {
        for (const auto& Comp : Components)
        {
            // 重みに応じて ABSORB_AMOUNT を分配
            float Ratio = Comp.Value / TotalWeight;
            int32 AddAmount = FMath::RoundToInt(ABSORB_AMOUNT * Ratio);

            int32& TankValue = ColorTankMap.FindOrAdd(Comp.Key);
            int32 OldValue = TankValue;
            TankValue = FMath::Min(TankValue + AddAmount, MAX_TANK_CAPACITY);

            UE_LOG(LogTemp, Log, TEXT("Absorbed %d to tank (was %d, now %d, max %d)"),
                AddAmount, OldValue, TankValue, MAX_TANK_CAPACITY);
        }
    }

    TargetComp->SetTargetColor(FLinearColor::White);
}