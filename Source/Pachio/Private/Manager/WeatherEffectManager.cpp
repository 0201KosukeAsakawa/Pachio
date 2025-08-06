// Fill out your copyright notice in the Description page of Project Settings.


#include "Manager/WeatherEffectManager.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"

UWeatherComponent::UWeatherComponent()
{
    PrimaryComponentTick.bCanEverTick = true; // 必要に応じてTickを使うならtrueに
}

void UWeatherComponent::BeginPlay()
{
    Super::BeginPlay();

    InitializeEffects();
    SetWeather(EWeatherType::Rain);
}

void UWeatherComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    AActor* Owner = GetOwner();
    if (!Owner) return;

    // プレイヤーキャラクター取得（例として最初のプレイヤーコントローラのPawn）
    APawn* PlayerPawn = GetWorld()->GetFirstPlayerController()->GetPawn();
    if (!PlayerPawn) return;

    FVector PlayerLocation = PlayerPawn->GetActorLocation();

    // 現在表示中のNiagaraコンポーネントをプレイヤー位置に移動
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

void UWeatherComponent::InitializeEffects()
{
    if (!GetOwner()) return;

    if (RainSystem && !RainEffect)
    {
        RainEffect = UNiagaraFunctionLibrary::SpawnSystemAttached(RainSystem, GetOwner()->GetRootComponent(), NAME_None, FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::KeepRelativeOffset, false);
        RainEffect->SetWorldScale3D(FVector(20.f));
        RainEffect->Deactivate();
    }
    if (ThunderSystem && !ThunderEffect)
    {
        ThunderEffect = UNiagaraFunctionLibrary::SpawnSystemAttached(ThunderSystem, GetOwner()->GetRootComponent(), NAME_None, FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::KeepRelativeOffset, false);
        ThunderEffect->SetWorldScale3D(FVector(20.f));
        ThunderEffect->Deactivate();
    }
    if (WindSystem && !WindEffect)
    {
        WindEffect = UNiagaraFunctionLibrary::SpawnSystemAttached(WindSystem, GetOwner()->GetRootComponent(), NAME_None, FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::KeepRelativeOffset, false);
        WindEffect->SetWorldScale3D(FVector(20.f));
        WindEffect->Deactivate();
    }
}

void UWeatherComponent::SetWeather(EWeatherType NewWeather)
{
    if (RainEffect) RainEffect->Deactivate();
    if (ThunderEffect) ThunderEffect->Deactivate();
    if (WindEffect) WindEffect->Deactivate();

    switch (NewWeather)
    {
    case EWeatherType::Rain:
        if (RainEffect) RainEffect->Activate();
        break;
    case EWeatherType::Thunder:
        if (ThunderEffect) ThunderEffect->Activate();
        break;
    case EWeatherType::Wind:
        if (WindEffect) WindEffect->Activate();
        break;
    case EWeatherType::Clear:
    default:
        break;
    }

    CurrentWeather = NewWeather;
}