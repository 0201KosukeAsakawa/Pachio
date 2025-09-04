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
	PrimaryComponentTick.bCanEverTick = true;
	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> FireflyBurst(TEXT("/Game/Niagara/FireflyBurst.FireflyBurst"));
	if (FireflyBurst.Succeeded())
	{
		FireflyBurstNiagaraSystem = FireflyBurst.Object;
	}
}

void UColorReactiveComponent::Init(UMeshComponent* mesh)
{
	if (!bSetStartColor || !mesh)
		return;

	AActor* Owner = GetOwner();
	if (!Owner)
		return;

	// StaticMeshComponent の取得
	USkeletalMeshComponent* MeshComp =
		UFunctionLibrary::FindComponentByName<USkeletalMeshComponent>(Owner, TEXT("Mesh"));
	if (!MeshComp)
		return;

	// Dynamic Material Instance 作成
	DynMesh = MeshComp->CreateAndSetMaterialInstanceDynamic(0);
	if (!DynMesh)
		return;

	// 色の取得（未登録なら白）
	CurrentColor = ALevelManager::GetInstance(GetWorld())
		->GetColorManager()
		->GetEffectColor(Effect);

	// マテリアルに色を反映
	DynMesh->SetVectorParameterValue(FName("BaseColor"), CurrentColor);
}


void UColorReactiveComponent::UpdateColorEffectAndNiagara(const FLinearColor& FilterColor,EBuffEffect newEffect, TArray<ANiagaraActor*>NiagaraComponents)
{
	CurrentColor = FilterColor;
	Effect = newEffect;
	Niagaras = NiagaraComponents;
}

bool UColorReactiveComponent::CheckColorMatch(FEffectMatchResult result,const FLinearColor& FilterColor, const bool buseComplementaryColor)
{
	FLinearColor CheckColor = FilterColor;  // コピーして変更可能に

	if (buseComplementaryColor)
	{
		CheckColor = GetComplementaryColor(CheckColor);  // 補色を取得して代入
		UE_LOG(LogTemp, Log, TEXT("CheckColor: %s"), *CheckColor.ToString());
	}

	float distance = ALevelManager::GetInstance(GetWorld())
		->GetColorManager()
		->GetColorDistanceRGB(CurrentColor, FilterColor);
		UE_LOG(LogTemp, Log, TEXT("Distance: %f"), distance);
	
	bool bMatch;
	if (distance <= 30.0f)
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

	float Max = FMath::Max3(R, G, B);
	float Min = FMath::Min3(R, G, B);
	float Delta = Max - Min;

	FHSLColor HSL;
	HSL.L = (Max + Min) / 2.0f;

	if (Delta == 0)
	{
		HSL.H = 0.0f;
		HSL.S = 0.0f;
	}
	else
	{
		HSL.S = (HSL.L < 0.5f) ? (Delta / (Max + Min)) : (Delta / (2.0f - Max - Min));

		if (Max == R)
			HSL.H = (G - B) / Delta + (G < B ? 6.0f : 0.0f);
		else if (Max == G)
			HSL.H = (B - R) / Delta + 2.0f;
		else
			HSL.H = (R - G) / Delta + 4.0f;

		HSL.H /= 6.0f;
	}
	return HSL;
}

// HSL → RGB 変換関数
FLinearColor HSLToRGB(const FHSLColor& HSL)
{
	float R, G, B;

	if (HSL.S == 0)
	{
		R = G = B = HSL.L; // 無彩色
	}
	else
	{
		auto HueToRGB = [](float p, float q, float t) -> float
			{
				if (t < 0.0f) t += 1.0f;
				if (t > 1.0f) t -= 1.0f;
				if (t < 1.0f / 6.0f) return p + (q - p) * 6.0f * t;
				if (t < 1.0f / 2.0f) return q;
				if (t < 2.0f / 3.0f) return p + (q - p) * (2.0f / 3.0f - t) * 6.0f;
				return p;
			};

		float q = (HSL.L < 0.5f) ? (HSL.L * (1 + HSL.S)) : (HSL.L + HSL.S - HSL.L * HSL.S);
		float p = 2 * HSL.L - q;

		R = HueToRGB(p, q, HSL.H + 1.0f / 3.0f);
		G = HueToRGB(p, q, HSL.H);
		B = HueToRGB(p, q, HSL.H - 1.0f / 3.0f);
	}

	return FLinearColor(R, G, B, 1.0f);
}

FLinearColor UColorReactiveComponent::GetComplementaryColor(const FLinearColor& InColor)
{
	// RGB → HSL に変換
	FHSLColor HSL = RGBToHSL(InColor);

	// 色相を180度反転（補色）
	HSL.H += 0.5f;
	if (HSL.H > 1.0f) HSL.H -= 1.0f;

	// パステル調に調整（彩度を低めに、明度は中～高）
	HSL.S = 0.3f;

	// 明度を 0.8～1.0 の範囲に収める（パステルに合わせる）
	HSL.L = FMath::Clamp(HSL.L, 0.8f, 1.0f);

	// HSL → RGB 変換
	FLinearColor Complementary = HSLToRGB(HSL);

	// 最大成分を取得
	float MaxComponent = FMath::Max3(Complementary.R, Complementary.G, Complementary.B);

	// 最大成分を1.0に強制
	if (Complementary.R == MaxComponent) Complementary.R = 1.0f;
	if (Complementary.G == MaxComponent) Complementary.G = 1.0f;
	if (Complementary.B == MaxComponent) Complementary.B = 1.0f;

	// 他の成分は0.8～1.0の範囲にクランプ
	if (Complementary.R != 1.0f) Complementary.R = FMath::Clamp(Complementary.R, 0.8f, 1.0f);
	if (Complementary.G != 1.0f) Complementary.G = FMath::Clamp(Complementary.G, 0.8f, 1.0f);
	if (Complementary.B != 1.0f) Complementary.B = FMath::Clamp(Complementary.B, 0.8f, 1.0f);

	Complementary.A = 1.0f;
	// ログ出力（RGBA各成分を表示）
	return Complementary;
}

void UColorReactiveComponent::SetSelectMode(bool bIsNowSelected)
{
	bSelected = bIsNowSelected;

	if (!DynMesh) return;

	if (!bSelected)
	{
		// 選択解除：発光を黒に
		DynMesh->SetVectorParameterValue(FName("EmissiveColor"), FLinearColor::Black);
	}
	// 選択ONのときはTickで動的に処理する
}

void UColorReactiveComponent::ToggleNiagaraActiveState(bool bVisible)
{
	if (GetOwner() == nullptr)
		return;

	TArray<ANiagaraActor*> NiagaraComponents = Niagaras;

	for (ANiagaraActor* NiagaraActor : NiagaraComponents)
	{
		if (NiagaraActor == nullptr)
			continue;
		NiagaraActor->SetActorHiddenInGame(!bVisible); // アクター自体の非表示
		NiagaraActor->SetActorEnableCollision(bVisible); // 当たり判定の有無（必要なら）

		UNiagaraComponent* NiagaraComp = NiagaraActor->GetNiagaraComponent();

		NiagaraComp->SetVisibility(bVisible, true); // コンポーネントの描画非表示
		NiagaraComp->SetPaused(!bVisible);          // シミュレーション停止
	}
}

void UColorReactiveComponent::ApplyColorToMaterial(FLinearColor InColor)
{
	AActor* Owner = GetOwner();
	if (!Owner)
		return;

	USkeletalMeshComponent* Mesh = UFunctionLibrary::FindComponentByName<USkeletalMeshComponent>(Owner, TEXT("Mesh"));
	if (!Mesh) return;

	UMaterialInstanceDynamic* DynMaterial = Mesh->CreateAndSetMaterialInstanceDynamic(0);
	if (!DynMaterial) return;

	DynMaterial->SetVectorParameterValue(FName("BaseColor"), InColor);
}

bool UColorReactiveComponent::IsColorMatch(const FLinearColor& FilterColor, const float Tolerance) const
{
    float dR = CurrentColor.R - FilterColor.R;
    float dG = CurrentColor.G - FilterColor.G;
    float dB = CurrentColor.B - FilterColor.B;

    // 人間の目に近い重み付き色差（輝度ベース）
    float ColorDifference = 0.299f * dR * dR + 0.587f * dG * dG + 0.114f * dB * dB;

    return ColorDifference <= Tolerance * Tolerance;
}

bool UColorReactiveComponent::IsColorMatch(const FLinearColor& FilterColor, const FLinearColor& TargetColor, const float Tolerance) const
{
	float dR = TargetColor.R - FilterColor.R;
	float dG = TargetColor.G - FilterColor.G;
	float dB = TargetColor.B - FilterColor.B;

	// 人間の目に近い重み付き色差（輝度ベース）
	float ColorDifference = 0.299f * dR * dR + 0.587f * dG * dG + 0.114f * dB * dB;

	return ColorDifference <= Tolerance * Tolerance;
}

void UColorReactiveComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!bSelected || !DynMesh) return;

	// Sine波で点滅（時間ベース）
	const float Time = GetWorld()->GetTimeSeconds();
	const float SineValue = (FMath::Sin(Time * 5.0f) + 1.0f) * 0.5f; // 0～1
	const float EmissiveStrength = 5.0f; // 最大輝度
	const FLinearColor EmissiveColor = CurrentColor * (SineValue * EmissiveStrength);

	DynMesh->SetVectorParameterValue(FName("EmissiveColor"), EmissiveColor);
}

bool UColorReactiveComponent::OnColorMatched(const FLinearColor& FilterColor)
{
	return true;
}

bool UColorReactiveComponent::OnColorMismatched(const FLinearColor& FilterColor)
{
	return false;
}

void UColorReactiveComponent::ActiveEffect()
{
	ActiveNiagaraEffect(FireflyBurstNiagaraSystem);
}

void UColorReactiveComponent::ActiveNiagaraEffect(UNiagaraSystem* niagaraSystem)
{
	if (!niagaraSystem)
	{
		UE_LOG(LogTemp, Warning, TEXT("NiagaraSystem is null"));
		return;
	}

	// アタッチするコンポーネントを取得（例: RootComponent）
	USkeletalMeshComponent* AttachComponent = UFunctionLibrary::FindComponentByName<USkeletalMeshComponent>(GetOwner(), TEXT("Mesh"));
	if (!AttachComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("AttachComponent is null"));
		return;
	}

	// Niagaraエフェクトをアタッチして再生
	UNiagaraFunctionLibrary::SpawnSystemAttached(
		niagaraSystem,
		AttachComponent,
		NAME_None,
		FVector::ZeroVector,
		FRotator::ZeroRotator,
		EAttachLocation::KeepRelativeOffset,
		true,   // bAutoDestroy
		true,   // bAutoActivate
		ENCPoolMethod::None,
		true    // bPreCullCheck
	);
}
