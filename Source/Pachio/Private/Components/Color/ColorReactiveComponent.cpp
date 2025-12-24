// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/Color/ColorReactiveComponent.h"
#include "NiagaraActor.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "Manager/LevelManager.h"
#include "Manager/ColorManager.h"
#include "FunctionLibrary.h"

// ====================================================================
// UColorReactiveComponent - 色処理のコアロジック層
// 色の計算・判定・エフェクト実行のみを担当
// マネージャーとの連携は一切行わない
// ====================================================================

UColorReactiveComponent::UColorReactiveComponent()
    :  bHide(false)
{
    PrimaryComponentTick.bCanEverTick = false;  // Tickは不要

    // Niagaraシステムのアセット参照を取得
    static ConstructorHelpers::FObjectFinder<UNiagaraSystem> FireflyBurst(
        TEXT("/Game/Niagara/FireflyBurst.FireflyBurst"));
    static ConstructorHelpers::FObjectFinder<UNiagaraSystem> ParticlesOfLight(
        TEXT("/Game/Niagara/ParticlesOfLight.ParticlesOfLight"));
    static ConstructorHelpers::FObjectFinder<UNiagaraSystem> LightCube(
        TEXT("/Game/Niagara/ParticleCube.ParticleCube"));

    if (FireflyBurst.Succeeded())
        FireflyBurstNiagaraSystem = FireflyBurst.Object;
    if (ParticlesOfLight.Succeeded())
        ParticlesOfLightNiagaraSystem = ParticlesOfLight.Object;
    if (LightCube.Succeeded())
        LightCubeNiagaraSystem = LightCube.Object;
}

void UColorReactiveComponent::ApplyColorWithMatching(const FLinearColor& NewColor)
{
    UObjectColorComponent::ApplyColorWithMatching(NewColor);
    if (UColorUtilityLibrary::IsHueSimilar(GetCurrentColor(), NewColor))
    {
        OnColorMatched(NewColor);
    }
    else
    {
        OnColorMismatched(NewColor);
    }
}