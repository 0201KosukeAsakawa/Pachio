// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/Color/ColorControllerComponent.h"
#include "Components/Color/ObjectColorComponent.h"
#include "DataContainer/ColorTargetTypes.h"
#include "FunctionLibrary.h"
#include "ColorUtilityLibrary.h"
#include "UI/UIManager.h"
#include "Manager/LevelManager.h"
#include "Manager/ColorManager.h"
#include "Kismet/KismetSystemLibrary.h" 

namespace
{
    constexpr int32 ABSORB_AMOUNT = 1;
    constexpr int32 MAX_TANK_CAPACITY = 1;
    constexpr int32 MIN_TANK_CAPACITY = 0;
}

// =======================
// コンストラクタ
// =======================
UColorControllerComponent::UColorControllerComponent()
{
    PrimaryComponentTick.bCanEverTick = true;

    // Tank順序を定義(赤・緑・青のみ)
    TankOrder.Add(EColorCategory::Red);
    TankOrder.Add(EColorCategory::Green);
    TankOrder.Add(EColorCategory::Blue);

    // 初期Tankマップ(RGB限定)
    ColorTankMap.Add(EColorCategory::Red, 0);
    ColorTankMap.Add(EColorCategory::Green, 0);
    ColorTankMap.Add(EColorCategory::Blue, 0);

    // 初期選択は0(赤)
    CurrentTankIndex = 0;
    CurrentColor = UColorUtilityLibrary::GetCategoryColor(EColorCategory::Red);
}

// =======================
// Tick
// =======================
void UColorControllerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
}

// =======================
// Tank切り替え
// =======================
void UColorControllerComponent::SwitchToNextTank()
{
    TArray<EColorCategory> AvailableTanks = GetAvailableTanks();

    if (AvailableTanks.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("No tanks available"));
        return;
    }

    // 現在のインデックスを次に進める(循環)
    CurrentTankIndex = (CurrentTankIndex + 1) % TankOrder.Num();

    // もし選択したTankが空なら、次の有効なTankを探す
    int32 SearchCount = 0;
    while (SearchCount < TankOrder.Num())
    {
        EColorCategory SelectedCategory = TankOrder[CurrentTankIndex];
        if (ColorTankMap[SelectedCategory] > 0)
        {
            UpdateColorFromCurrentTank();
            OnColorChanged.Broadcast(CurrentColor);
            UE_LOG(LogTemp, Log, TEXT("Switched to tank index %d"), CurrentTankIndex);
            return;
        }

        CurrentTankIndex = (CurrentTankIndex + 1) % TankOrder.Num();
        SearchCount++;
    }

    UE_LOG(LogTemp, Warning, TEXT("No valid tanks found during switch"));
}

void UColorControllerComponent::SwitchToPreviousTank()
{
    TArray<EColorCategory> AvailableTanks = GetAvailableTanks();

    if (AvailableTanks.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("No tanks available"));
        return;
    }

    // 現在のインデックスを前に戻す(循環)
    CurrentTankIndex = (CurrentTankIndex - 1 + TankOrder.Num()) % TankOrder.Num();

    // もし選択したTankが空なら、前の有効なTankを探す
    int32 SearchCount = 0;
    while (SearchCount < TankOrder.Num())
    {
        EColorCategory SelectedCategory = TankOrder[CurrentTankIndex];
        if (ColorTankMap[SelectedCategory] > 0)
        {
            UpdateColorFromCurrentTank();
            OnColorChanged.Broadcast(CurrentColor);
            UE_LOG(LogTemp, Log, TEXT("Switched to tank index %d"), CurrentTankIndex);
            return;
        }

        CurrentTankIndex = (CurrentTankIndex - 1 + TankOrder.Num()) % TankOrder.Num();
        SearchCount++;
    }

    UE_LOG(LogTemp, Warning, TEXT("No valid tanks found during switch"));
}

TArray<EColorCategory> UColorControllerComponent::GetAvailableTanks() const
{
    TArray<EColorCategory> AvailableTanks;

    for (EColorCategory Category : TankOrder)
    {
        if (ColorTankMap.Contains(Category) && ColorTankMap[Category] > 0)
        {
            AvailableTanks.Add(Category);
        }
    }

    return AvailableTanks;
}

void UColorControllerComponent::UpdateColorFromCurrentTank()
{
    if (!TankOrder.IsValidIndex(CurrentTankIndex))
    {
        return;
    }

    EColorCategory SelectedCategory = TankOrder[CurrentTankIndex];
    FLinearColor NewColor = UColorUtilityLibrary::GetCategoryColor(SelectedCategory);
    CurrentColor = FLinearColor(NewColor.R, NewColor.G, NewColor.B, CurrentColor.A);
}

void UColorControllerComponent::ConsumeTank(EColorCategory Category, int32 Amount)
{
    if (!ColorTankMap.Contains(Category))
    {
        return;
    }

    int32& TankValue = ColorTankMap[Category];
    int32 OldValue = TankValue;
    TankValue = FMath::Max(TankValue - Amount, MIN_TANK_CAPACITY);

    UE_LOG(LogTemp, Log, TEXT("Consumed %d from tank, remaining: %d"), Amount, TankValue);

    // ★現在選択中のTankが空になった場合、次のTankに自動切り替え
    if (TankValue <= 0 && TankOrder.IsValidIndex(CurrentTankIndex))
    {
        EColorCategory CurrentCategory = TankOrder[CurrentTankIndex];
        if (CurrentCategory == Category)
        {
            UE_LOG(LogTemp, Warning, TEXT("Current tank is empty, switching to next available tank"));

            // 次に残量がある色を探す
            TArray<EColorCategory> AvailableTanks = GetAvailableTanks();
            if (AvailableTanks.Num() > 0)
            {
                SwitchToNextTank();
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("All tanks are empty!"));
            }
        }
    }
}

int32 UColorControllerComponent::GetTankAmount(EColorCategory Category) const
{
    if (ColorTankMap.Contains(Category))
    {
        return ColorTankMap[Category];
    }
    return 0;
}

// =======================
// 色調整
// =======================
void UColorControllerComponent::AdjustColor(float Delta)
{
    // 現在のTankの色を基準に調整
    if (!TankOrder.IsValidIndex(CurrentTankIndex))
    {
        return;
    }

    UpdateColorFromCurrentTank();
}

FLinearColor UColorControllerComponent::GetCurrentColor() const
{
    return CurrentColor;
}

UObjectColorComponent* UColorControllerComponent::GetHitColorComponent(float Range)
{
    FVector Center = GetOwner()->GetActorLocation();
    FVector BoxExtent(Range);

    TArray<AActor*> ActorsToIgnore;
    ActorsToIgnore.Add(GetOwner());

    TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
    ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldDynamic));
    ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldStatic));

    TArray<AActor*> OverlappedActors;

    bool bOverlapped = UKismetSystemLibrary::BoxOverlapActors(
        this,
        Center,
        BoxExtent,
        ObjectTypes,
        AActor::StaticClass(),
        ActorsToIgnore,
        OverlappedActors
    );

    if (!bOverlapped)
    {
        return nullptr;
    }

    UObjectColorComponent* NearestComponent = nullptr;
    float NearestDistanceSq = FLT_MAX;

    for (AActor* Actor : OverlappedActors)
    {
        if (!Actor)
        {
            continue;
        }

        if (UObjectColorComponent* ColorComp = Actor->GetComponentByClass<UObjectColorComponent>())
        {
            const float DistSq = FVector::DistSquared(Center, Actor->GetActorLocation());

            if (DistSq < NearestDistanceSq)
            {
                NearestDistanceSq = DistSq;
                NearestComponent = ColorComp;
            }
        }
    }

    return NearestComponent;
}

void UColorControllerComponent::PaintHitObject(UObjectColorComponent* TargetComp)
{
    if (!TargetComp) return;

    // 現在選択中のTankの色でペイント
    if (!TankOrder.IsValidIndex(CurrentTankIndex))
    {
        return;
    }

    EColorCategory CurrentCategory = TankOrder[CurrentTankIndex];

    // Tankの残量チェック
    if (ColorTankMap[CurrentCategory] <= 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot paint: tank is empty"));
        return;
    }

    FLinearColor TraceColor = UColorUtilityLibrary::GetCategoryColor(CurrentCategory);
    TargetComp->SetTargetColor(TraceColor);

    // Tank残量を消費
    ConsumeTank(CurrentCategory, 1);
}

void UColorControllerComponent::AbsorbHitObject(UObjectColorComponent* TargetComp)
{
    if (!TargetComp) return;

    if (TargetComp->IsPainting())
    {
        UE_LOG(LogTemp, Warning, TEXT("Target is currently painting, cannot absorb"));
        return;
    }

    FLinearColor HitColor = TargetComp->GetCurrentColor();
    FVector HSL = UColorUtilityLibrary::GetHSL(HitColor);
    float Saturation = HSL.Y;

    // 無彩色は吸収しない(RGBのみ)
    if (Saturation < 0.01f)
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot absorb colorless objects"));
        return;
    }

    // RGB成分を抽出してTankに加算
    TArray<TPair<EColorCategory, float>> Components;

    if (HitColor.R > 0.7f)
    {
        Components.Add({ EColorCategory::Red, HitColor.R });
    }
    if (HitColor.G > 0.7f)
    {
        Components.Add({ EColorCategory::Green, HitColor.G });
    }
    if (HitColor.B > 0.7f)
    {
        Components.Add({ EColorCategory::Blue, HitColor.B });
    }

    float TotalWeight = 0.0f;
    for (const auto& Comp : Components)
    {
        TotalWeight += Comp.Value;
    }

    if (TotalWeight > 0.0f)
    {
        for (const auto& Comp : Components)
        {
            float Ratio = Comp.Value / TotalWeight;
            int32 AddAmount = FMath::RoundToInt(ABSORB_AMOUNT * Ratio);

            int32& TankValue = ColorTankMap[Comp.Key];
            int32 OldValue = TankValue;
            TankValue = FMath::Min(TankValue + AddAmount, MAX_TANK_CAPACITY);

            UE_LOG(LogTemp, Log, TEXT("Absorbed %d to tank (was %d, now %d)"),
                AddAmount, OldValue, TankValue);
        }
    }

    TargetComp->SetTargetColor(FLinearColor::White);
}