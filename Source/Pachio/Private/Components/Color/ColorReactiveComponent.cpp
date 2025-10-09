// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/Color/ColorReactiveComponent.h"
#include "NiagaraActor.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "Manager/LevelManager.h"
#include "Manager/ColorManager.h"
#include "FunctionLibrary.h"

// Sets default values for this component's properties
UColorReactiveComponent::UColorReactiveComponent()
{
	// コンポーネントのTickを有効化
	PrimaryComponentTick.bCanEverTick = true;

	// Niagaraシステムのアセット参照を取得
	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> FireflyBurst(TEXT("/Game/Niagara/FireflyBurst.FireflyBurst"));
	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> ParticlesOfLight(TEXT("/Game/Niagara/ParticlesOfLight.ParticlesOfLight"));
	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> LightCube(TEXT("/Game/Niagara/ParticleCube.ParticleCube"));

	// アセットの読み込みに成功した場合、メンバ変数に保存
	if (FireflyBurst.Succeeded())
	{
		FireflyBurstNiagaraSystem = FireflyBurst.Object;
	}

	if (ParticlesOfLight.Succeeded())
	{
		ParticlesOfLightNiagaraSystem = ParticlesOfLight.Object;
	}

	if (LightCube.Succeeded())
	{
		LightCubeNiagaraSystem = LightCube.Object;
	}
}

void UColorReactiveComponent::Init(bool Variable)
{
	// 開始色の設定フラグがfalseの場合は初期化しない
	if (!bSetStartColor)
		return;

	// オーナーアクターの取得
	AActor* Owner = GetOwner();
	if (!Owner)
		return;

	// SkeletalMeshComponentの取得
	USkeletalMeshComponent* MeshComp =
		UFunctionLibrary::FindComponentByName<USkeletalMeshComponent>(Owner, TEXT("Mesh"));
	if (!MeshComp)
		return;

	// Dynamic Material Instanceの作成
	// マテリアルスロット0に対してダイナミックマテリアルを生成
	constexpr int32 MaterialSlotIndex = 0;
	DynMesh = MeshComp->CreateAndSetMaterialInstanceDynamic(MaterialSlotIndex);
	if (!DynMesh)
		return;

	// エフェクトに対応する色をカラーマネージャーから取得
	CurrentColor = ALevelManager::GetInstance(GetWorld())
		->GetColorManager()
		->GetEffectColor(Effect);

	// Variable=falseの場合、即座にマテリアルに色を反映
	if (!Variable)
	{
		DynMesh->SetVectorParameterValue(FName("BaseColor"), CurrentColor);
	}
}

void UColorReactiveComponent::InitColorEffectAndNiagara(const FLinearColor& FilterColor, EBuffEffect newEffect, TArray<ANiagaraActor*> NiagaraComponents)
{
	// 初期カラー、エフェクトタイプ、Niagaraアクター配列を設定
	CurrentColor = FilterColor;
	Effect = newEffect;
	Niagaras = NiagaraComponents;
}

bool UColorReactiveComponent::IsRGBDistancewithinThreshold(FEffectMatchResult result, const FLinearColor& FilterColor, const bool bUseComplementaryColor)
{
	// 比較用の色を設定（補色を使用する場合は補色を計算）
	FLinearColor CheckColor = FilterColor;

	if (bUseComplementaryColor)
	{
		CheckColor = GetComplementaryColor(FilterColor);
		UE_LOG(LogTemp, Log, TEXT("Using Complementary CheckColor: %s"), *CheckColor.ToString());
	}

	// RGB空間での色の距離を計算
	float distance = ALevelManager::GetInstance(GetWorld())
		->GetColorManager()
		->GetColorDistanceRGB(CurrentColor, CheckColor);

	// 色の一致判定用の閾値
	constexpr float ColorMatchThreshold = 30.0f;

	// 距離が閾値以下なら色が一致、そうでなければ不一致
	bool bMatch;
	if (distance <= ColorMatchThreshold)
	{
		bMatch = OnColorMatched(CheckColor);
	}
	else
	{
		bMatch = OnColorMismatched(CheckColor);
	}
	return bMatch;
}

// RGB → HSL 変換関数
FHSLColor RGBToHSL(const FLinearColor& Color)
{
	float R = Color.R;
	float G = Color.G;
	float B = Color.B;

	// RGB値の最大値と最小値を取得
	float Max = FMath::Max3(R, G, B);
	float Min = FMath::Min3(R, G, B);
	float Delta = Max - Min;

	FHSLColor HSL;

	// 明度（Lightness）の計算
	constexpr float LightnessScale = 0.5f;
	HSL.L = (Max + Min) * LightnessScale;

	// 無彩色の場合
	if (Delta == 0)
	{
		HSL.H = 0.0f;  // 色相は未定義だが0とする
		HSL.S = 0.0f;  // 彩度は0
	}
	else
	{
		// 彩度（Saturation）の計算
		constexpr float SaturationThreshold = 0.5f;
		constexpr float SaturationDenominator = 2.0f;
		HSL.S = (HSL.L < SaturationThreshold)
			? (Delta / (Max + Min))
			: (Delta / (SaturationDenominator - Max - Min));

		// 色相（Hue）の計算
		constexpr float HueOffset = 6.0f;
		constexpr float HueGreenOffset = 2.0f;
		constexpr float HueBlueOffset = 4.0f;
		constexpr float HueDivisor = 6.0f;

		if (Max == R)
			HSL.H = (G - B) / Delta + (G < B ? HueOffset : 0.0f);
		else if (Max == G)
			HSL.H = (B - R) / Delta + HueGreenOffset;
		else
			HSL.H = (R - G) / Delta + HueBlueOffset;

		HSL.H /= HueDivisor;
	}
	return HSL;
}

// HSL → RGB 変換関数
FLinearColor HSLToRGB(const FHSLColor& HSL)
{
	float R, G, B;

	// 彩度が0の場合は無彩色（グレースケール）
	if (HSL.S == 0)
	{
		R = G = B = HSL.L;
	}
	else
	{
		// 色相から RGB を計算するヘルパー関数
		auto HueToRGB = [](float p, float q, float t) -> float
			{
				// 色相値を0～1の範囲に正規化
				if (t < 0.0f) t += 1.0f;
				if (t > 1.0f) t -= 1.0f;

				// 色相の区間に応じて線形補間
				constexpr float OneSixth = 1.0f / 6.0f;
				constexpr float OneHalf = 1.0f / 2.0f;
				constexpr float TwoThirds = 2.0f / 3.0f;
				constexpr float HueMultiplier = 6.0f;

				if (t < OneSixth) return p + (q - p) * HueMultiplier * t;
				if (t < OneHalf) return q;
				if (t < TwoThirds) return p + (q - p) * (TwoThirds - t) * HueMultiplier;
				return p;
			};

		// 明度と彩度から中間値を計算
		constexpr float LightnessThreshold = 0.5f;
		constexpr float LightnessScale = 2.0f;
		float q = (HSL.L < LightnessThreshold)
			? (HSL.L * (1 + HSL.S))
			: (HSL.L + HSL.S - HSL.L * HSL.S);
		float p = LightnessScale * HSL.L - q;

		// 各RGBチャンネルを計算
		constexpr float HueRedOffset = 1.0f / 3.0f;
		constexpr float HueBlueOffset = 1.0f / 3.0f;
		R = HueToRGB(p, q, HSL.H + HueRedOffset);
		G = HueToRGB(p, q, HSL.H);
		B = HueToRGB(p, q, HSL.H - HueBlueOffset);
	}

	constexpr float FullAlpha = 1.0f;
	return FLinearColor(R, G, B, FullAlpha);
}

FLinearColor UColorReactiveComponent::GetComplementaryColor(const FLinearColor& InColor)
{
	// RGB → HSL に変換
	FHSLColor HSL = RGBToHSL(InColor);

	// 色相を180度（0.5）反転して補色を作成
	constexpr float ComplementaryHueOffset = 0.5f;
	constexpr float MaxHue = 1.0f;
	HSL.H += ComplementaryHueOffset;
	if (HSL.H > MaxHue)
		HSL.H -= MaxHue;

	// パステル調に調整するための彩度と明度の設定
	constexpr float PastelSaturation = 0.3f;
	constexpr float MinPastelLightness = 0.8f;
	constexpr float MaxPastelLightness = 1.0f;

	HSL.S = PastelSaturation;
	HSL.L = FMath::Clamp(HSL.L, MinPastelLightness, MaxPastelLightness);

	// HSL → RGB 変換
	FLinearColor Complementary = HSLToRGB(HSL);

	// 最大成分を取得
	float MaxComponent = FMath::Max3(Complementary.R, Complementary.G, Complementary.B);

	// 最大成分を1.0に強制してビビッドさを保つ
	constexpr float MaxComponentValue = 1.0f;
	constexpr float MinComponentValue = 0.8f;

	if (Complementary.R == MaxComponent) Complementary.R = MaxComponentValue;
	if (Complementary.G == MaxComponent) Complementary.G = MaxComponentValue;
	if (Complementary.B == MaxComponent) Complementary.B = MaxComponentValue;

	// 他の成分は0.8～1.0の範囲にクランプしてパステル調を維持
	if (Complementary.R != MaxComponentValue)
		Complementary.R = FMath::Clamp(Complementary.R, MinComponentValue, MaxComponentValue);
	if (Complementary.G != MaxComponentValue)
		Complementary.G = FMath::Clamp(Complementary.G, MinComponentValue, MaxComponentValue);
	if (Complementary.B != MaxComponentValue)
		Complementary.B = FMath::Clamp(Complementary.B, MinComponentValue, MaxComponentValue);

	constexpr float FullAlpha = 1.0f;
	Complementary.A = FullAlpha;

	return Complementary;
}

void UColorReactiveComponent::SetSelectMode(bool bIsNowSelected)
{
	// 選択状態を更新
	bSelected = bIsNowSelected;

	if (!DynMesh) return;

	// 選択解除時は発光を無効化
	if (!bSelected)
	{
		DynMesh->SetVectorParameterValue(FName("EmissiveColor"), FLinearColor::Black);
	}
	// 選択中の発光エフェクトはTickComponentで処理
}

void UColorReactiveComponent::ActiveEffect(bool bActive)
{
	// 全てのNiagaraアクターのアクティブ状態を切り替え
	for (ANiagaraActor* Niagara : Niagaras)
	{
		if (!Niagara) continue;

		// 表示・非表示を切り替え（bActiveがtrueなら表示）
		Niagara->SetActorHiddenInGame(!bActive);
		Niagara->SetActorEnableCollision(bActive);
		Niagara->SetActorTickEnabled(bActive);
	}
}

void UColorReactiveComponent::ToggleNiagaraActiveState(bool bVisible)
{
	// オーナーの存在確認
	if (GetOwner() == nullptr)
		return;

	// 各Niagaraアクターの表示状態を切り替え
	for (ANiagaraActor* NiagaraActor : Niagaras)
	{
		if (NiagaraActor == nullptr)
			continue;

		// アクター自体の表示・非表示
		NiagaraActor->SetActorHiddenInGame(!bVisible);
		NiagaraActor->SetActorEnableCollision(bVisible);

		// Niagaraコンポーネントの取得と状態変更
		UNiagaraComponent* NiagaraComp = NiagaraActor->GetNiagaraComponent();
		if (NiagaraComp)
		{
			// コンポーネントの可視性とシミュレーション状態を制御
			NiagaraComp->SetVisibility(bVisible, true);
			NiagaraComp->SetPaused(!bVisible);
		}
	}
}

void UColorReactiveComponent::ApplyColorToMaterial(FLinearColor InColor)
{
	// オーナーアクターの取得
	AActor* Owner = GetOwner();
	if (!Owner)
		return;

	// SkeletalMeshComponentの取得
	USkeletalMeshComponent* Mesh = UFunctionLibrary::FindComponentByName<USkeletalMeshComponent>(Owner, TEXT("Mesh"));
	if (!Mesh) return;

	// Dynamic Material Instanceを作成
	constexpr int32 MaterialSlotIndex = 0;
	UMaterialInstanceDynamic* DynMaterial = Mesh->CreateAndSetMaterialInstanceDynamic(MaterialSlotIndex);
	if (!DynMaterial) return;

	// マテリアルのベースカラーパラメータに色を設定
	DynMaterial->SetVectorParameterValue(FName("BaseColor"), InColor);
}

bool UColorReactiveComponent::IsColorDegreeDistanceWithinThreshold(const FLinearColor& FilterColor, const float Tolerance) const
{
	// RGB差分を計算
	float dR = CurrentColor.R - FilterColor.R;
	float dG = CurrentColor.G - FilterColor.G;
	float dB = CurrentColor.B - FilterColor.B;

	// 人間の視覚特性に基づいた重み付き色差（ITU-R BT.601係数）
	constexpr float RedWeight = 0.299f;
	constexpr float GreenWeight = 0.587f;
	constexpr float BlueWeight = 0.114f;

	float ColorDifference = RedWeight * dR * dR + GreenWeight * dG * dG + BlueWeight * dB * dB;

	// 許容範囲内かどうかを判定
	return ColorDifference <= Tolerance * Tolerance;
}

bool UColorReactiveComponent::IsColorDegreeDistanceWithinThreshold(const FLinearColor& FilterColor, const FLinearColor& TargetColor, const float Tolerance) const
{
	// RGB差分を計算
	float dR = TargetColor.R - FilterColor.R;
	float dG = TargetColor.G - FilterColor.G;
	float dB = TargetColor.B - FilterColor.B;

	// 人間の視覚特性に基づいた重み付き色差（ITU-R BT.601係数）
	constexpr float RedWeight = 0.299f;
	constexpr float GreenWeight = 0.587f;
	constexpr float BlueWeight = 0.114f;

	float ColorDifference = RedWeight * dR * dR + GreenWeight * dG * dG + BlueWeight * dB * dB;

	// 許容範囲内かどうかを判定
	return ColorDifference <= Tolerance * Tolerance;
}

void UColorReactiveComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// 選択時の発光エフェクトなど、フレーム単位の処理が必要な場合はここに実装
}

bool UColorReactiveComponent::OnColorMatched(const FLinearColor& FilterColor)
{
	// 色が一致した際の処理（派生クラスでオーバーライド可能）
	return true;
}

bool UColorReactiveComponent::OnColorMismatched(const FLinearColor& FilterColor)
{
	// 色が不一致だった際の処理（派生クラスでオーバーライド可能）
	return false;
}

void UColorReactiveComponent::PlayAppearEffect()
{
	// 出現時のエフェクトを再生
	ActiveNiagaraEffect(FireflyBurstNiagaraSystem);
	ActiveNiagaraEffect(LightCubeNiagaraSystem);
}

void UColorReactiveComponent::ActiveNiagaraEffect(UNiagaraSystem* niagaraSystem)
{
	// Niagaraシステムの存在確認
	if (!niagaraSystem)
	{
		UE_LOG(LogTemp, Warning, TEXT("NiagaraSystem is null"));
		return;
	}

	// アタッチ先のSkeletalMeshComponentを取得
	USkeletalMeshComponent* AttachComponent = UFunctionLibrary::FindComponentByName<USkeletalMeshComponent>(GetOwner(), TEXT("Mesh"));
	if (!AttachComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("AttachComponent is null"));
		return;
	}

	// NiagaraシステムをSkeletalMeshにアタッチして生成
	UNiagaraComponent* targetNiagara = UNiagaraFunctionLibrary::SpawnSystemAttached(
		niagaraSystem,
		AttachComponent,
		NAME_None,
		FVector::ZeroVector,
		FRotator::ZeroRotator,
		EAttachLocation::KeepRelativeOffset,
		true,  // bAutoDestroy
		true,  // bAutoActivate
		ENCPoolMethod::None,
		true   // bPreCullCheck
	);

	if (targetNiagara)
	{
		// RGB値の最大成分を見つける
		float maxRGB = FMath::Max3(CurrentColor.R, CurrentColor.G, CurrentColor.B);

		FLinearColor targetColor = CurrentColor;

		// エフェクトの輝度を上げるための乗算係数
		constexpr float EmissiveMultiplier = 50.0f;

		// 最大成分のみを強調してエフェクトを鮮やかにする
		if (CurrentColor.R == maxRGB)
		{
			targetColor.R *= EmissiveMultiplier;
		}
		else if (CurrentColor.G == maxRGB)
		{
			targetColor.G *= EmissiveMultiplier;
		}
		else
		{
			targetColor.B *= EmissiveMultiplier;
		}

		// Niagaraのユーザーパラメータに色を設定
		targetNiagara->SetVariableLinearColor(FName("User_Color"), targetColor);

		// アクティブなエフェクトリストに追加（後でDeactivateするため）
		ActiveNiagaraComponent.Add(targetNiagara);
	}
}

void UColorReactiveComponent::DeactivateAllEffects()
{
	// 全てのアクティブなNiagaraコンポーネントを停止・破棄
	for (UNiagaraComponent* NiagaraComp : ActiveNiagaraComponent)
	{
		if (NiagaraComp && !NiagaraComp->IsBeingDestroyed())
		{
			NiagaraComp->Deactivate();
			NiagaraComp->DestroyComponent();
		}
	}

	// リストをクリア
	ActiveNiagaraComponent.Empty();
}