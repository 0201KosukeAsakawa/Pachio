// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/Color/ColorControllerComponent.h"
#include "Components/Color/ObjectColorComponent.h"

#include "DataContainer/ColorTargetTypes.h"
#include "FunctionLibrary.h"

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

    // カラーマップ初期化（Responders / Event は除外）
    const TArray<EColorTargetType> AllModes = UFunctionLibrary::GetAllEnumValues<EColorTargetType>();
    for (EColorTargetType Mode : AllModes)
    {
        if (Mode == EColorTargetType::Responders || Mode == EColorTargetType::Event)
            continue;

        // 初期色は白
        ColorMap.Add(Mode, FLinearColor::White);
    }
}

// =======================
// Tick（フレーム更新処理）
// =======================

void UColorControllerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    // レイの開始位置と終了位置を設定
    FVector Start = GetOwner()->GetActorLocation();
    FVector Direction = GetOwner()->GetActorRightVector().GetSafeNormal();
    FVector End = Start + Direction * 1500.f; // 5.f だと短すぎるので500に変更（任意）

    FHitResult HitResult;

    // オーナーを無視するアクターリストに追加
    TArray<AActor*> ActorsToIgnore;
    ActorsToIgnore.Add(GetOwner());
    
    FLinearColor TraceColor = ColorMap[CurrentColorMode];

    // LineTraceSingle 実行
    // LineTraceSingle 実行（デバッグカラー指定版）
    bool bHit = UKismetSystemLibrary::LineTraceSingle(
        this,
        Start,
        End,
        UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_WorldStatic),
        false,
        ActorsToIgnore,
        EDrawDebugTrace::ForDuration, // デバッグ描画オン
        HitResult,
        true,
        TraceColor,
        TraceColor,
        0.5f // 表示時間
    );

    // ヒットしたら処理
    if (bHit && HitResult.GetActor())
    {
        AActor* HitActor = HitResult.GetActor();

        // 例えば、UMyComponent クラスを持っているか確認
        if (UObjectColorComponent* ri = HitActor->GetComponentByClass<UObjectColorComponent>())
        {
            UE_LOG(LogTemp, Log, TEXT("%s has UMyComponent!"), *HitActor->GetName());

            // ここに処理を書く
            ri->ApplyColorWithMatching(TraceColor);
        }
    }

}

// =======================
// 色調整系
// =======================

float GetRoundedAngle(float X)
{
    // 最も近い60の倍数に丸めて360で正規化
    float Rounded = FMath::RoundToFloat(X / 60.0f) * 60.0f;
    float Result = FMath::Fmod(Rounded, 360.0f);

    // Fmodは負の結果を返すことがあるので正に補正
    if (Result < 0.0f)
        Result += 360.0f;

    return Result;
}

void UColorControllerComponent::AdjustColor(float Delta)
{
    // 現在モードの色を HSV に変換
    FLinearColor HSV = ColorMap[CurrentColorMode].LinearRGBToHSV();

    float Hue = Data.R;                                  // 色相
    float Saturation = FMath::Clamp(HSV.G, 0.1f, 0.3f); // 彩度（固定範囲に制限）
    float Value = FMath::Clamp(HSV.B, 0.8f, 1.0f);      // 明度（明るめを維持）

    // Hue を Delta 分だけ回転
    Hue = FMath::Fmod(Hue + Delta * 360.0f, 360.0f);
    if (Hue < 0.f) 
        Hue += 360.f;

    Data = UColorUtilityLibrary::FromHue(Hue);
    UE_LOG(LogTemp, Log, TEXT("Hue : : %f"), Hue);
    float f = GetRoundedAngle(Hue);
    UE_LOG(LogTemp, Log, TEXT("f : %f"), f);
    // HSV → RGB に戻す
    FLinearColor NewColor = FLinearColor(f, Saturation, Value).HSVToLinearRGB();

    // 現在モードの色を更新（αは保持）
    ColorMap[CurrentColorMode] = FLinearColor(NewColor.R, NewColor.G, NewColor.B, ColorMap[CurrentColorMode].A);

    // イベントを通知
    //OnColorChanged.Broadcast(ColorMap[CurrentColorMode], CurrentColorMode);
}

void UColorControllerComponent::SetColor(float Value)
{
    // 現在モードの色を HSV に変換
    FLinearColor HSV = ColorMap[CurrentColorMode].LinearRGBToHSV();

    float Hue = HSV.R;                                 // 色相
    float Saturation = FMath::Clamp(HSV.G, 0.2f, 0.6f); // 彩度
    float Brightness = FMath::Clamp(HSV.B, 0.8f, 1.0f); // 明度

    // Hue を直接設定
    Hue = FMath::Fmod(Value, 360.0f);
    if (Hue < 0.f) Hue += 360.f;

    // HSV → RGB に戻す
    FLinearColor NewColor = FLinearColor(Hue, Saturation, Brightness).HSVToLinearRGB();

    // 現在モードの色を更新（αは保持）
    ColorMap[CurrentColorMode] = FLinearColor(NewColor.R, NewColor.G, NewColor.B, ColorMap[CurrentColorMode].A);

    // イベントを通知
    OnColorChanged.Broadcast(ColorMap[CurrentColorMode], CurrentColorMode);
}

// =======================
// モード切替系
// =======================

void UColorControllerComponent::ChangeMode(int Direction)
{
    // Direction を +1 / -1 に正規化
    Direction = (Direction >= 1) ? 1 : -1;

    // 次のモードを取得
    EColorTargetType NextMode = (Direction > 0)
        ? GetNextMode(CurrentColorMode)
        : GetPreviousMode(CurrentColorMode);

    UE_LOG(LogTemp, Warning, TEXT("Trying Mode Change: %d -> %d"), static_cast<int32>(CurrentColorMode), static_cast<int32>(NextMode));

    // ObjectColor は特殊処理、それ以外はシンプル処理
    if (NextMode == EColorTargetType::ObjectColor)
    {
        HandleObjectColorMode(Direction, NextMode);
        return;
    }

    HandleSimpleMode(Direction, NextMode);
}

void UColorControllerComponent::HandleObjectColorMode(int Direction, EColorTargetType NextMode)
{
    // 必要なマネージャ取得
    ALevelManager* LevelManager = ALevelManager::GetInstance(GetWorld());
    UColorManager* ColorManager = LevelManager ? LevelManager->GetColorManager() : nullptr;
    if (!LevelManager || !ColorManager)
    {
        UE_LOG(LogTemp, Warning, TEXT("LevelManager または ColorManager が取得できませんでした"));
        return;
    }

    // 最も近い対象を探索
    IColorReactiveInterface* ClosestTarget = nullptr;
    AActor* TargetActor = nullptr;
    if (!FindClosestColorTarget(ClosestTarget, TargetActor))
    {
        UE_LOG(LogTemp, Warning, TEXT("対象が見つからなかったため、モードを変更しませんでした"));
        return;
    }

    // モード更新
    CurrentColorMode = NextMode;
    UE_LOG(LogTemp, Warning, TEXT("New Mode: %d"), static_cast<int32>(CurrentColorMode));

    // カラーマネージャに対象を設定
    ColorManager->SetColorTarget(ClosestTarget);
    UE_LOG(LogTemp, Warning, TEXT("ColorTarget を ColorManager に設定しました"));

    // UI にマーカーを表示
    if (TargetActor)
        LevelManager->GetUIManager()->ShowMarker(TEXT("ChangeColorTarget"), TargetActor);

    // アニメーションコールバック実行
    if (AnimationDelegate.IsBound())
        AnimationDelegate.Execute(Direction);
}

void UColorControllerComponent::HandleSimpleMode(int Direction, EColorTargetType NextMode)
{
    // 必要なマネージャ取得
    ALevelManager* LevelManager = ALevelManager::GetInstance(GetWorld());
    UColorManager* ColorManager = LevelManager ? LevelManager->GetColorManager() : nullptr;

    // モード更新
    CurrentColorMode = NextMode;
    UE_LOG(LogTemp, Warning, TEXT("New Mode: %d"), static_cast<int32>(CurrentColorMode));

    // ターゲットをリセットし、UIマーカーを非表示
    if (ColorManager) ColorManager->ResetColorTarget();
    if (LevelManager) LevelManager->GetUIManager()->HideMarker(TEXT("ChangeColorTarget"));

    // アニメーションコールバック実行
    if (AnimationDelegate.IsBound())
        AnimationDelegate.Execute(Direction);
}

// =======================
// ターゲット探索処理
// =======================

bool UColorControllerComponent::FindClosestColorTarget(IColorReactiveInterface*& OutTarget, AActor*& OutActor)
{
    // 自身の位置を基準に探索
    FVector Start = GetOwner()->GetActorLocation();
    FVector BoxExtent(1000.f, 1000.f, 1000.f); // 探索範囲

    TArray<FHitResult> HitResults;
    FCollisionShape Box = FCollisionShape::MakeBox(BoxExtent);
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(GetOwner());

    // Box 内でヒット対象を検索
    bool bHit = GetWorld()->SweepMultiByChannel(
        HitResults, Start, Start, FQuat::Identity, ECC_Visibility, Box, Params
    );
    if (!bHit) return false;

    float ClosestDistSq = TNumericLimits<float>::Max();
    OutTarget = nullptr;
    OutActor = nullptr;

    // ヒットしたアクターを走査
    for (const FHitResult& Hit : HitResults)
    {
        AActor* HitActor = Hit.GetActor();
        if (!HitActor || !HitActor->GetClass()->ImplementsInterface(UColorReactiveInterface::StaticClass()))
            continue;

        // インターフェースにキャスト
        IColorReactiveInterface* IR = Cast<IColorReactiveInterface>(HitActor);
        if (!IR || !IR->IsChangeable())
            continue;

        // 最も近い対象を選択
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

// =======================
// モード取得処理
// =======================

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
    // Responders / Event を除外したモード一覧を作成
    TArray<EColorTargetType> FilteredModes;
    for (EColorTargetType Mode : UFunctionLibrary::GetAllEnumValues<EColorTargetType>())
    {
        if (Mode != EColorTargetType::Responders && Mode != EColorTargetType::Event)
            FilteredModes.Add(Mode);
    }

    // 現在モードのインデックスを取得
    int32 CurrentIndex = FilteredModes.IndexOfByKey(CurrentMode);
    if (CurrentIndex == INDEX_NONE)
        return EColorTargetType::WorldColor;

    // 指定方向へ循環移動
    int32 NewIndex = (CurrentIndex + Direction + FilteredModes.Num()) % FilteredModes.Num();
    return FilteredModes[NewIndex];
}
