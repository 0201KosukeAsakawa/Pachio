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
// UColorReactiveComponent - 色処理・エフェクト実行層
// 管理者(UObjectColorComponent)からの指示のみで動作する
// ====================================================================

UColorReactiveComponent::UColorReactiveComponent()
{
    PrimaryComponentTick.bCanEverTick = true;

    // Niagaraシステムのアセット参照を取得
    static ConstructorHelpers::FObjectFinder<UNiagaraSystem> FireflyBurst(TEXT("/Game/Niagara/FireflyBurst.FireflyBurst"));
    static ConstructorHelpers::FObjectFinder<UNiagaraSystem> ParticlesOfLight(TEXT("/Game/Niagara/ParticlesOfLight.ParticlesOfLight"));
    static ConstructorHelpers::FObjectFinder<UNiagaraSystem> LightCube(TEXT("/Game/Niagara/ParticleCube.ParticleCube"));

    if (FireflyBurst.Succeeded()) FireflyBurstNiagaraSystem = FireflyBurst.Object;
    if (ParticlesOfLight.Succeeded()) ParticlesOfLightNiagaraSystem = ParticlesOfLight.Object;
    if (LightCube.Succeeded()) LightCubeNiagaraSystem = LightCube.Object;
}

// =======================
// 初期化 (最小限)
// =======================

void UColorReactiveComponent::Initialize(FLinearColor InitialColor)
{
    if (!bSetStartColor) return;

    AActor* Owner = GetOwner();
    if (!Owner) return;

    USkeletalMeshComponent* MeshComp = UFunctionLibrary::FindComponentByName<USkeletalMeshComponent>(Owner, TEXT("Mesh"));
    if (!MeshComp) return;

    // ダイナミックマテリアルを生成（色適用用）
    constexpr int32 MaterialSlotIndex = 0;
    DynMesh = MeshComp->CreateAndSetMaterialInstanceDynamic(MaterialSlotIndex);

    CurrentColor = InitialColor;
}

// =======================
// エフェクト・Niagara設定
// =======================

void UColorReactiveComponent::SetupNiagaraActors(TArray<ANiagaraActor*> InNiagaraActors)
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

void UColorReactiveComponent::ApplyColorToMaterial(FLinearColor InColor)
{
    if (!DynMesh) return;

    CurrentColor = InColor;
    DynMesh->SetVectorParameterValue(FName("BaseColor"), InColor);
}

// =======================
// 色判定 (Pure Logic)
// =======================

bool UColorReactiveComponent::IsRGBDistanceWithinThreshold(
    const FLinearColor& ColorA,
    const FLinearColor& ColorB,
    float Threshold) const
{
    // RGB空間での距離計算
    float dR = ColorA.R - ColorB.R;
    float dG = ColorA.G - ColorB.G;
    float dB = ColorA.B - ColorB.B;

    float distance = FMath::Sqrt(dR * dR + dG * dG + dB * dB);
    return distance <= Threshold;
}

bool UColorReactiveComponent::IsColorDegreeDistanceWithinThreshold(
    const FLinearColor& ColorA,
    const FLinearColor& ColorB,
    float Tolerance) const
{
    // 人間の視覚特性に基づいた重み付き色差（ITU-R BT.601係数）
    float dR = ColorA.R - ColorB.R;
    float dG = ColorA.G - ColorB.G;
    float dB = ColorA.B - ColorB.B;

    constexpr float RedWeight = 0.299f;
    constexpr float GreenWeight = 0.587f;
    constexpr float BlueWeight = 0.114f;

    float ColorDifference = RedWeight * dR * dR + GreenWeight * dG * dG + BlueWeight * dB * dB;
    return ColorDifference <= Tolerance * Tolerance;
}

bool UColorReactiveComponent::IsColorDegreeDistanceWithinThreshold(
    const FLinearColor& FilterColor,
    float Tolerance) const
{
    return IsColorDegreeDistanceWithinThreshold(CurrentColor, FilterColor, Tolerance);
}

// =======================
// 補色計算
// =======================

FLinearColor UColorReactiveComponent::GetComplementaryColor(const FLinearColor& InColor)
{
    FHSLColor HSL = RGBToHSL(InColor);

    // 色相を180度反転
    constexpr float ComplementaryHueOffset = 0.5f;
    HSL.H += ComplementaryHueOffset;
    if (HSL.H > 1.0f) HSL.H -= 1.0f;

    // パステル調に調整
    constexpr float PastelSaturation = 0.3f;
    constexpr float MinPastelLightness = 0.8f;
    constexpr float MaxPastelLightness = 1.0f;

    HSL.S = PastelSaturation;
    HSL.L = FMath::Clamp(HSL.L, MinPastelLightness, MaxPastelLightness);

    FLinearColor Complementary = HSLToRGB(HSL);

    // 最大成分を1.0に強制
    float MaxComponent = FMath::Max3(Complementary.R, Complementary.G, Complementary.B);
    constexpr float MaxComponentValue = 1.0f;
    constexpr float MinComponentValue = 0.8f;

    if (Complementary.R == MaxComponent) Complementary.R = MaxComponentValue;
    if (Complementary.G == MaxComponent) Complementary.G = MaxComponentValue;
    if (Complementary.B == MaxComponent) Complementary.B = MaxComponentValue;

    if (Complementary.R != MaxComponentValue)
        Complementary.R = FMath::Clamp(Complementary.R, MinComponentValue, MaxComponentValue);
    if (Complementary.G != MaxComponentValue)
        Complementary.G = FMath::Clamp(Complementary.G, MinComponentValue, MaxComponentValue);
    if (Complementary.B != MaxComponentValue)
        Complementary.B = FMath::Clamp(Complementary.B, MinComponentValue, MaxComponentValue);

    Complementary.A = 1.0f;
    return Complementary;
}

// =======================
// 選択モード
// =======================

void UColorReactiveComponent::SetSelectMode(bool bIsNowSelected)
{
    bSelected = bIsNowSelected;

    if (!DynMesh) return;

    if (!bSelected)
    {
        DynMesh->SetVectorParameterValue(FName("EmissiveColor"), FLinearColor::Black);
    }
}

// =======================
// Niagaraエフェクト制御
// =======================

void UColorReactiveComponent::ToggleNiagaraActiveState(bool bVisible)
{
    if (!GetOwner()) return;

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
    ActiveNiagaraEffect(FireflyBurstNiagaraSystem);
    ActiveNiagaraEffect(LightCubeNiagaraSystem);
}

void UColorReactiveComponent::ActiveNiagaraEffect(UNiagaraSystem* niagaraSystem)
{
    if (!niagaraSystem)
    {
        UE_LOG(LogTemp, Warning, TEXT("NiagaraSystem is null"));
        return;
    }

    USkeletalMeshComponent* AttachComponent = UFunctionLibrary::FindComponentByName<USkeletalMeshComponent>(GetOwner(), TEXT("Mesh"));
    if (!AttachComponent)
    {
        UE_LOG(LogTemp, Warning, TEXT("AttachComponent is null"));
        return;
    }

    UNiagaraComponent* targetNiagara = UNiagaraFunctionLibrary::SpawnSystemAttached(
        niagaraSystem,
        AttachComponent,
        NAME_None,
        FVector::ZeroVector,
        FRotator::ZeroRotator,
        EAttachLocation::KeepRelativeOffset,
        true, true,
        ENCPoolMethod::None,
        true
    );

    if (targetNiagara)
    {
        // エフェクト用の色を強調
        float maxRGB = FMath::Max3(CurrentColor.R, CurrentColor.G, CurrentColor.B);
        FLinearColor targetColor = CurrentColor;
        constexpr float EmissiveMultiplier = 50.0f;

        if (CurrentColor.R == maxRGB) targetColor.R *= EmissiveMultiplier;
        else if (CurrentColor.G == maxRGB) targetColor.G *= EmissiveMultiplier;
        else targetColor.B *= EmissiveMultiplier;

        targetNiagara->SetVariableLinearColor(FName("User_Color"), targetColor);
        ActiveNiagaraComponent.Add(targetNiagara);
    }
}

void UColorReactiveComponent::DeactivateAllEffects()
{
    for (UNiagaraComponent* NiagaraComp : ActiveNiagaraComponent)
    {
        if (NiagaraComp && !NiagaraComp->IsBeingDestroyed())
        {
            NiagaraComp->Deactivate();
            NiagaraComp->DestroyComponent();
        }
    }
    ActiveNiagaraComponent.Empty();
}

// =======================
// Tick処理
// =======================

void UColorReactiveComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    // 選択時の発光エフェクトなど、必要に応じて実装
}

// =======================
// RGB ⇔ HSL 変換 (ユーティリティ)
// =======================

FHSLColor RGBToHSL(const FLinearColor& Color)
{
    float R = Color.R, G = Color.G, B = Color.B;
    float Max = FMath::Max3(R, G, B);
    float Min = FMath::Min3(R, G, B);
    float Delta = Max - Min;

    FHSLColor HSL;
    HSL.L = (Max + Min) * 0.5f;

    if (Delta == 0)
    {
        HSL.H = 0.0f;
        HSL.S = 0.0f;
    }
    else
    {
        HSL.S = (HSL.L < 0.5f) ? (Delta / (Max + Min)) : (Delta / (2.0f - Max - Min));

        if (Max == R) HSL.H = (G - B) / Delta + (G < B ? 6.0f : 0.0f);
        else if (Max == G) HSL.H = (B - R) / Delta + 2.0f;
        else HSL.H = (R - G) / Delta + 4.0f;

        HSL.H /= 6.0f;
    }
    return HSL;
}

FLinearColor HSLToRGB(const FHSLColor& HSL)
{
    float R, G, B;

    if (HSL.S == 0)
    {
        R = G = B = HSL.L;
    }
    else
    {
        auto HueToRGB = [](float p, float q, float t) -> float
            {
                if (t < 0.0f) t += 1.0f;
                if (t > 1.0f) t -= 1.0f;

                if (t < 1.0f / 6.0f) return p + (q - p) * 6.0f * t;
                if (t < 0.5f) return q;
                if (t < 2.0f / 3.0f) return p + (q - p) * (2.0f / 3.0f - t) * 6.0f;
                return p;
            };

        float q = (HSL.L < 0.5f) ? (HSL.L * (1 + HSL.S)) : (HSL.L + HSL.S - HSL.L * HSL.S);
        float p = 2.0f * HSL.L - q;

        R = HueToRGB(p, q, HSL.H + 1.0f / 3.0f);
        G = HueToRGB(p, q, HSL.H);
        B = HueToRGB(p, q, HSL.H - 1.0f / 3.0f);
    }

    return FLinearColor(R, G, B, 1.0f);
}