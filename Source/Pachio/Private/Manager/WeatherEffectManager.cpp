// Fill out your copyright notice in the Description page of Project Settings.


#include "Manager/WeatherEffectManager.h"
#include "Manager/ColorManager.h"
#include "Manager/LevelManager.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Logic/ColorManager/ColorTargetRegistry.h"
#include "Components/Color/ColorReactiveComponent.h"

// =======================
// コンストラクタ
// =======================

UWeatherComponent::UWeatherComponent()
{
    // 毎フレームTickを使うので true
    PrimaryComponentTick.bCanEverTick = true;
}

// =======================
// 初期化処理
// =======================

void UWeatherComponent::BeginPlay()
{
    Super::BeginPlay();

    // 天候用エフェクトの初期化
    InitializeEffects();

    // 色変化イベントに反応するよう登録
    ALevelManager::GetInstance(GetWorld())
        ->GetColorManager()
        ->GetColorTargetRegistry()
        ->OnColorApplied.AddDynamic(this, &UWeatherComponent::SetWeather);
}

// =======================
// 毎フレーム更新処理
// =======================

void UWeatherComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    AActor* Owner = GetOwner();
    if (!Owner) return;

    // プレイヤーキャラクターを取得
    APawn* PlayerPawn = GetWorld()->GetFirstPlayerController()->GetPawn();
    if (!PlayerPawn) return;

    FVector PlayerLocation = PlayerPawn->GetActorLocation();

    // 現在有効な天候エフェクトをプレイヤー位置に追従させる
    switch (CurrentWeather)
    {
    case EWeatherType::Rain:
        if (RainEffect)
            RainEffect->SetWorldLocation(PlayerLocation);
        break;
    case EWeatherType::Thunder:
        if (ThunderEffect)
            ThunderEffect->SetWorldLocation(PlayerLocation);
        break;
    case EWeatherType::Wind:
        if (WindEffect)
            WindEffect->SetWorldLocation(PlayerLocation);
        break;
    default:
        break;
    }
}

// =======================
// 天候エフェクト初期化
// =======================

void UWeatherComponent::InitializeEffects()
{
    if (!GetOwner()) return;

    // 雨エフェクト生成（最初は非表示）
    if (RainSystem && !RainEffect)
    {
        RainEffect = UNiagaraFunctionLibrary::SpawnSystemAttached(
            RainSystem,
            GetOwner()->GetRootComponent(),
            NAME_None,
            FVector::ZeroVector,
            FRotator::ZeroRotator,
            EAttachLocation::KeepRelativeOffset,
            false
        );
        RainEffect->SetWorldScale3D(FVector(20.f));
        RainEffect->Deactivate();
    }

    // 雷エフェクト生成（最初は非表示）
    if (ThunderSystem && !ThunderEffect)
    {
        ThunderEffect = UNiagaraFunctionLibrary::SpawnSystemAttached(
            ThunderSystem,
            GetOwner()->GetRootComponent(),
            NAME_None,
            FVector::ZeroVector,
            FRotator::ZeroRotator,
            EAttachLocation::KeepRelativeOffset,
            false
        );
        ThunderEffect->SetWorldScale3D(FVector(20.f));
        ThunderEffect->Deactivate();
    }

    // 風エフェクト生成（最初は非表示）
    if (WindSystem && !WindEffect)
    {
        WindEffect = UNiagaraFunctionLibrary::SpawnSystemAttached(
            WindSystem,
            GetOwner()->GetRootComponent(),
            NAME_None,
            FVector::ZeroVector,
            FRotator::ZeroRotator,
            EAttachLocation::KeepRelativeOffset,
            false
        );
        WindEffect->SetWorldScale3D(FVector(20.f));
        WindEffect->Deactivate();
    }
}

// =======================
// 天候設定処理（色に応じて切り替え）
// =======================

void UWeatherComponent::SetWeather( FLinearColor NewColor)
{
    // 色から最も近いエフェクト種別を取得
    EColorCategory Match =  UColorUtilityLibrary::GetNearestPrimaryColor(NewColor);

    // すべてのエフェクトを一旦停止
    if (RainEffect) RainEffect->Deactivate();
    if (ThunderEffect) ThunderEffect->Deactivate();
    if (WindEffect) WindEffect->Deactivate();

    // デフォルトは晴れ
    EWeatherType NewWeather = EWeatherType::Clear;

    // 色に応じて天候を切り替える
    switch (Match)
    {
    case EColorCategory::Red: // 赤 → 雷
        if (ThunderEffect)
        {
            ThunderEffect->Activate();
            NewWeather = EWeatherType::Thunder;
        }
        break;

    case EColorCategory::Green: // 緑 → 風
        if (WindEffect)
        {
            WindEffect->Activate();
            NewWeather = EWeatherType::Wind;
        }
        break;

    case EColorCategory::Blue: // 青 → 雨
        if (RainEffect)
        {
            RainEffect->Activate();
            NewWeather = EWeatherType::Rain;
        }
        break;

    default:
        break;
    }

    // 現在の天候を更新
    CurrentWeather = NewWeather;
}
