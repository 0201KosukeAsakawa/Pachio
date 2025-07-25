// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/ColorControllerComponent.h"
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
    ALevelManager* LevelManager = ALevelManager::GetInstance(GetWorld());
    UColorManager* ColorManager = LevelManager->GetColorManager();

    // Directionが1以上なら1、それ未満なら-1に補正
    Direction = (Direction >= 1) ? 1 : -1;

    EColorTargetType NextMode = CurrentColorMode;

    // Direction が正のときは次、負のときは前
    if (Direction > 0)
    {
        NextMode = GetNextMode(CurrentColorMode);
    }
    else
    {
        NextMode = GetPreviousMode(CurrentColorMode);
    }

    // 一時ログ
    UE_LOG(LogTemp, Warning, TEXT("Trying Mode Change: %d -> %d"), static_cast<int32>(CurrentColorMode), static_cast<int32>(NextMode));

    if (NextMode == EColorTargetType::ObjectColor)
    {
        FVector Start = GetOwner()->GetActorLocation();
        FVector BoxExtent(1000.f, 1000.f, 1000.f);
        FVector End = Start;

        TArray<FHitResult> HitResults;
        FCollisionShape Box = FCollisionShape::MakeBox(BoxExtent);
        FCollisionQueryParams Params;
        Params.AddIgnoredActor(GetOwner());

        bool bHit = GetWorld()->SweepMultiByChannel(
            HitResults,
            Start,
            End,
            FQuat::Identity,
            ECC_Visibility,
            Box,
            Params
        );

        IColorReactiveInterface* ClosestTarget = nullptr;
        float ClosestDistSq = TNumericLimits<float>::Max();
        AActor* TargetActor = nullptr;

        for (const FHitResult& Hit : HitResults)
        {
            AActor* HitActor = Hit.GetActor();
            if (!HitActor) continue;

            if (HitActor->GetClass()->ImplementsInterface(UColorReactiveInterface::StaticClass()))
            {
                IColorReactiveInterface* ir = Cast<IColorReactiveInterface>(HitActor);
                if (!ir /*|| ir->IsColorModifiable()*/)
                    continue;

                float DistSq = FVector::DistSquared(HitActor->GetActorLocation(), Start);
                if (DistSq < ClosestDistSq)
                {
                    ClosestDistSq = DistSq;
                    ClosestTarget = ir;
                    TargetActor = HitActor;
                }
            }
        }

        if (ClosestTarget)
        {
            // 対象が見つかったのでモード確定
            CurrentColorMode = NextMode;
            UE_LOG(LogTemp, Warning, TEXT("New Mode: %d"), static_cast<int32>(CurrentColorMode));

            if (LevelManager)
            {
                if (ColorManager)
                {
                    ColorManager->SetColorTarget(ClosestTarget);
                    UE_LOG(LogTemp, Warning, TEXT("ColorTarget を ColorManager に設定しました"));

                    if (TargetActor)
                        LevelManager->GetUIManager()->ShowMarker(TEXT("ChangeColorTarget"), TargetActor);
                }
                else
                {
                    UE_LOG(LogTemp, Warning, TEXT("ColorTarget が取得できませんでした"));
                }
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("LevelManager が取得できませんでした"));
            }

            AnimationDelegate.Execute(Direction);
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("対象が見つからなかったため、モードを変更しませんでした"));
        }
    }
    else
    {
        // 対象を必要としないモードならそのまま切り替え
        CurrentColorMode = NextMode;
        UE_LOG(LogTemp, Warning, TEXT("New Mode: %d"), static_cast<int32>(CurrentColorMode));
        ColorManager->ResetColorTarget();
        ALevelManager::GetInstance(GetWorld())->GetUIManager()->HideMarker(TEXT("ChangeColorTarget"));
        AnimationDelegate.Execute(Direction);
    }
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
        return EColorTargetType::WorldColor; // デフォルトに戻す
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
        return EColorTargetType::WorldColor; // デフォルトに戻す
    }

    return FilteredModes[(CurrentIndex - 1 + FilteredModes.Num()) % FilteredModes.Num()];
}
