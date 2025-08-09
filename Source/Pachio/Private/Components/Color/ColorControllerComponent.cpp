// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/Color/ColorControllerComponent.h"
#include "DataContainer/EffectMatchResult.h"
#include "FunctionLibrary.h"
#include "UI/ColorLens.h"
#include "UI/UIManager.h"
#include "Manager/LevelManager.h"
#include "Manager/ColorManager.h"


// Sets default values for this component's properties
UColorControllerComponent::UColorControllerComponent()
{
    PrimaryComponentTick.bCanEverTick = true;

    // カラーマップ初期化（Responders/Event は除外）
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
    FLinearColor HSV = ColorMap[CurrentColorMode].LinearRGBToHSV();

    float Hue = HSV.R;
    float Saturation = FMath::Clamp(HSV.G, 0.2f, 0.6f); // 彩度
    float Value = FMath::Clamp(HSV.B, 0.8f, 1.0f);      // 明度

    Hue = FMath::Fmod(Hue + Delta * 360.0f, 360.0f);
    if (Hue < 0.f) Hue += 360.f;

    FLinearColor NewColor = FLinearColor(Hue, Saturation, Value).HSVToLinearRGB();
    ColorMap[CurrentColorMode] = FLinearColor(NewColor.R, NewColor.G, NewColor.B, ColorMap[CurrentColorMode].A);

    OnColorChanged.Broadcast(ColorMap[CurrentColorMode], CurrentColorMode);
}

void UColorControllerComponent::ChangeMode(int Direction)
{
    Direction = (Direction >= 1) ? 1 : -1;
    EColorTargetType NextMode = (Direction > 0)
        ? GetNextMode(CurrentColorMode)
        : GetPreviousMode(CurrentColorMode);

    UE_LOG(LogTemp, Warning, TEXT("Trying Mode Change: %d -> %d"), static_cast<int32>(CurrentColorMode), static_cast<int32>(NextMode));

    if (NextMode == EColorTargetType::ObjectColor)
    {
        HandleObjectColorMode(Direction, NextMode);
        return;
    }

    HandleSimpleMode(Direction, NextMode);
}

void UColorControllerComponent::HandleObjectColorMode(int Direction, EColorTargetType NextMode)
{
    ALevelManager* LevelManager = ALevelManager::GetInstance(GetWorld());
    UColorManager* ColorManager = LevelManager ? LevelManager->GetColorManager() : nullptr;
    if (!LevelManager || !ColorManager)
    {
        UE_LOG(LogTemp, Warning, TEXT("LevelManager または ColorManager が取得できませんでした"));
        return;
    }

    IColorReactiveInterface* ClosestTarget = nullptr;
    AActor* TargetActor = nullptr;
    if (!FindClosestColorTarget(ClosestTarget, TargetActor))
    {
        UE_LOG(LogTemp, Warning, TEXT("対象が見つからなかったため、モードを変更しませんでした"));
        return;
    }

    CurrentColorMode = NextMode;
    UE_LOG(LogTemp, Warning, TEXT("New Mode: %d"), static_cast<int32>(CurrentColorMode));

    ColorManager->SetColorTarget(ClosestTarget);
    UE_LOG(LogTemp, Warning, TEXT("ColorTarget を ColorManager に設定しました"));

    if (TargetActor)
        LevelManager->GetUIManager()->ShowMarker(TEXT("ChangeColorTarget"), TargetActor);

    if(AnimationDelegate.IsBound())
    AnimationDelegate.Execute(Direction);
}

void UColorControllerComponent::HandleSimpleMode(int Direction, EColorTargetType NextMode)
{
    ALevelManager* LevelManager = ALevelManager::GetInstance(GetWorld());
    UColorManager* ColorManager = LevelManager ? LevelManager->GetColorManager() : nullptr;

    CurrentColorMode = NextMode;
    UE_LOG(LogTemp, Warning, TEXT("New Mode: %d"), static_cast<int32>(CurrentColorMode));

    if (ColorManager) ColorManager->ResetColorTarget();
    if (LevelManager) LevelManager->GetUIManager()->HideMarker(TEXT("ChangeColorTarget"));
    if (AnimationDelegate.IsBound())
    AnimationDelegate.Execute(Direction);
}

bool UColorControllerComponent::FindClosestColorTarget(IColorReactiveInterface*& OutTarget, AActor*& OutActor)
{
    FVector Start = GetOwner()->GetActorLocation();
    FVector BoxExtent(1000.f, 1000.f, 1000.f);

    TArray<FHitResult> HitResults;
    FCollisionShape Box = FCollisionShape::MakeBox(BoxExtent);
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(GetOwner());

    bool bHit = GetWorld()->SweepMultiByChannel(
        HitResults, Start, Start, FQuat::Identity, ECC_Visibility, Box, Params
    );
    if (!bHit) return false;

    float ClosestDistSq = TNumericLimits<float>::Max();
    OutTarget = nullptr;
    OutActor = nullptr;

    for (const FHitResult& Hit : HitResults)
    {
        AActor* HitActor = Hit.GetActor();
        if (!HitActor || !HitActor->GetClass()->ImplementsInterface(UColorReactiveInterface::StaticClass()))
            continue;

        IColorReactiveInterface* IR = Cast<IColorReactiveInterface>(HitActor);
        if (!IR || !IR->IsChangeable())
            continue;

        float DistSq = FVector::DistSquared(HitActor->GetActorLocation(), Start);
        if (DistSq < ClosestDistSq)
        {
            ClosestDistSq = DistSq;
            OutTarget = IR;
            OutActor = HitActor;
        }
    }
    return OutTarget != nullptr;
}

EColorTargetType UColorControllerComponent::GetNextMode(EColorTargetType CurrentMode)
{
    return GetAdjacentMode(CurrentMode, +1);
}

EColorTargetType UColorControllerComponent::GetPreviousMode(EColorTargetType CurrentMode)
{
    return GetAdjacentMode(CurrentMode, -1);
}

EColorTargetType UColorControllerComponent::GetAdjacentMode(EColorTargetType CurrentMode, int Direction)
{
    TArray<EColorTargetType> FilteredModes;
    for (EColorTargetType Mode : UFunctionLibrary::GetAllEnumValues<EColorTargetType>())
    {
        if (Mode != EColorTargetType::Responders && Mode != EColorTargetType::Event)
            FilteredModes.Add(Mode);
    }

    int32 CurrentIndex = FilteredModes.IndexOfByKey(CurrentMode);
    if (CurrentIndex == INDEX_NONE)
        return EColorTargetType::WorldColor;

    int32 NewIndex = (CurrentIndex + Direction + FilteredModes.Num()) % FilteredModes.Num();
    return FilteredModes[NewIndex];
}