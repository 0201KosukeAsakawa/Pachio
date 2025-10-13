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
    : CurrentColor(FLinearColor::White)
    , Effect(EBuffEffect::Red)
    , bSelected(false)
    , bHide(false)
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

// =======================
// 初期化
// =======================

void UColorReactiveComponent::Initialize(const FLinearColor& InitialColor, bool bVariable, AActor* Owner)
{
    AActor* TargetOwner = Owner ? Owner : GetOwner();
    if (!TargetOwner)
    {
        UE_LOG(LogTemp, Warning, TEXT("ColorReactiveComponent: Owner is null in Initialize"));
        return;
    }

    USkeletalMeshComponent* MeshComp = UFunctionLibrary::FindComponentByName<USkeletalMeshComponent>(
        TargetOwner, TEXT("Mesh"));
    if (!MeshComp)
    {
        UE_LOG(LogTemp, Warning, TEXT("ColorReactiveComponent: Mesh component not found on %s"),
            *TargetOwner->GetName());
        return;
    }

    // ダイナミックマテリアルを生成
    constexpr int32 MaterialSlotIndex = 0;
    DynMesh = MeshComp->CreateAndSetMaterialInstanceDynamic(MaterialSlotIndex);

    if (!DynMesh)
    {
        UE_LOG(LogTemp, Error, TEXT("ColorReactiveComponent: Failed to create dynamic material on %s"),
            *TargetOwner->GetName());
        return;
    }

    CurrentColor = InitialColor;
    DynMesh->SetVectorParameterValue(FName("BaseColor"), InitialColor);

    UE_LOG(LogTemp, Log, TEXT("ColorReactiveComponent initialized on %s with color (R:%.2f, G:%.2f, B:%.2f)"),
        *TargetOwner->GetName(), InitialColor.R, InitialColor.G, InitialColor.B);
}

void UColorReactiveComponent::SetupNiagaraActors(const TArray<ANiagaraActor*>& InNiagaraActors)
{
    Niagaras = InNiagaraActors;
}

void UColorReactiveComponent::SetEffectType(EBuffEffect NewEffect)
{
    Effect = NewEffect;
}

// =======================
// 色の適用
// =======================

void UColorReactiveComponent::ApplyColorToMaterial(const FLinearColor& InColor)
{
    if (!DynMesh)
    {
        UE_LOG(LogTemp, Warning, TEXT("ColorReactiveComponent: DynMesh is null in ApplyColorToMaterial"));
        return;
    }

    CurrentColor = InColor;
    DynMesh->SetVectorParameterValue(FName("BaseColor"), InColor);
}

// =======================
// 状態管理
// =======================

void UColorReactiveComponent::SetSelectMode(bool bIsNowSelected)
{
    bSelected = bIsNowSelected;

    if (!DynMesh) return;

    if (bSelected)
    {
        // 選択時のエミッシブ効果（例: 白い発光）
        DynMesh->SetVectorParameterValue(FName("EmissiveColor"), FLinearColor::White * 0.5f);
    }
    else
    {
        // 選択解除時はエミッシブをオフ
        DynMesh->SetVectorParameterValue(FName("EmissiveColor"), FLinearColor::Black);
    }
}

void UColorReactiveComponent::SetHidden(bool bInHide)
{
    bHide = bInHide;

    // Niagaraエフェクトも連動して非表示
    ToggleNiagaraActiveState(!bHide);
}

// =======================
// Niagaraエフェクト制御
// =======================

void UColorReactiveComponent::ToggleNiagaraActiveState(bool bVisible)
{
    for (ANiagaraActor* NiagaraActor : Niagaras)
    {
        if (!NiagaraActor) continue;

        NiagaraActor->SetActorHiddenInGame(!bVisible);
        NiagaraActor->SetActorEnableCollision(bVisible);

        UNiagaraComponent* NiagaraComp = NiagaraActor->GetNiagaraComponent();
        if (NiagaraComp)
        {
            NiagaraComp->SetVisibility(bVisible, true);
            NiagaraComp->SetPaused(!bVisible);
        }
    }
}

void UColorReactiveComponent::PlayAppearEffect()
{
    ActivateNiagaraEffect(FireflyBurstNiagaraSystem);
    ActivateNiagaraEffect(LightCubeNiagaraSystem);
}

void UColorReactiveComponent::ActivateNiagaraEffect(UNiagaraSystem* NiagaraSystem)
{
    if (!NiagaraSystem)
    {
        UE_LOG(LogTemp, Warning, TEXT("ColorReactiveComponent: NiagaraSystem is null"));
        return;
    }

    AActor* Owner = GetOwner();
    if (!Owner)
    {
        UE_LOG(LogTemp, Warning, TEXT("ColorReactiveComponent: Owner is null in ActivateNiagaraEffect"));
        return;
    }

    USkeletalMeshComponent* AttachComponent = UFunctionLibrary::FindComponentByName<USkeletalMeshComponent>(
        Owner, TEXT("Mesh"));
    if (!AttachComponent)
    {
        UE_LOG(LogTemp, Warning, TEXT("ColorReactiveComponent: Attach component not found"));
        return;
    }

    UNiagaraComponent* TargetNiagara = UNiagaraFunctionLibrary::SpawnSystemAttached(
        NiagaraSystem,
        AttachComponent,
        NAME_None,
        FVector::ZeroVector,
        FRotator::ZeroRotator,
        EAttachLocation::KeepRelativeOffset,
        true, true,
        ENCPoolMethod::None,
        true
    );

    if (TargetNiagara)
    {
        // エフェクト用の色を強調（UColorUtilityLibraryを使用）
        const FLinearColor EnhancedColor = UColorUtilityLibrary::EnhanceMaxComponent(
            CurrentColor,
            50.0f
        );

        TargetNiagara->SetVariableLinearColor(FName("User_Color"), EnhancedColor);
        ActiveNiagaraComponents.Add(TargetNiagara);
    }
}

void UColorReactiveComponent::DeactivateAllEffects()
{
    for (UNiagaraComponent* NiagaraComp : ActiveNiagaraComponents)
    {
        if (NiagaraComp && !NiagaraComp->IsBeingDestroyed())
        {
            NiagaraComp->Deactivate();
            NiagaraComp->DestroyComponent();
        }
    }
    ActiveNiagaraComponents.Empty();
}

// =======================
// 色マッチングコールバック（派生クラスで拡張可能）
// =======================

bool UColorReactiveComponent::OnColorMatched(const FLinearColor& FilterColor)
{
    // デフォルト実装: 何もしない
    // 派生クラスでオーバーライドして独自の処理を実装可能
    return true;
}

bool UColorReactiveComponent::OnColorMismatched(const FLinearColor& FilterColor)
{
    // デフォルト実装: 何もしない
    // 派生クラスでオーバーライドして独自の処理を実装可能
    return true;
}