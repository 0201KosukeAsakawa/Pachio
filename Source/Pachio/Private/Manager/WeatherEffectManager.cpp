// Fill out your copyright notice in the Description page of Project Settings.


#include "Manager/WeatherEffectManager.h"
#include "Manager/ColorManager.h"
#include "Manager/LevelManager.h"
#include "DataContainer/BuffEffect.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Logic/ColorManager/ColorTargetRegistry.h"

UWeatherComponent::UWeatherComponent()
{
    PrimaryComponentTick.bCanEverTick = true; // 必要に応じてTickを使うならtrueに
}

void UWeatherComponent::BeginPlay()
{
    Super::BeginPlay();
    InitializeEffects();
    ALevelManager::GetInstance(GetWorld())->GetColorManager()->GetColorTargetRegistry()->OnColorApplied.AddDynamic(this, &UWeatherComponent::SetWeather);
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

void UWeatherComponent::SetWeather(EColorTargetType Mode,FLinearColor NewColor)
{
    if (Mode != EColorTargetType::WorldColor)
        return;

    FEffectMatchResult Match = ALevelManager::GetInstance(GetWorld())
        ->GetColorManager()
        ->GetClosestEffectByHue(NewColor);
    if (RainEffect) RainEffect->Deactivate();
    if (ThunderEffect) ThunderEffect->Deactivate();
    if (WindEffect) WindEffect->Deactivate();
    EWeatherType NewWeather = EWeatherType::Clear;
    switch (Match.ClosestEffect)
    {
    case EBuffEffect::Red:
    {
        if (ThunderEffect)
        {
            ThunderEffect->Activate();
            NewWeather = EWeatherType::Clear;
        }
        break;
    }
        
    case EBuffEffect::Green:
    {
        if (WindEffect)
        {
            WindEffect->Activate();
            NewWeather = EWeatherType::Wind;
        }
        break;
    }
    case EBuffEffect::Blue:
        if (RainEffect)
        {
            RainEffect->Activate();
            NewWeather = EWeatherType::Rain;
        }
        break;
    default:
        break;
    }

    CurrentWeather = NewWeather;
}