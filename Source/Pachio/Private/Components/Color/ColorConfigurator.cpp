#include "Components/Color/ColorConfigurator.h"
#include "Components/Color/ColorReactiveComponent.h"
#include "Components/Beat/BeatScalerComponent.h"
#include "Interface/ColorFilterInterface.h"
#include "Manager/LevelManager.h"
#include "Manager/ColorManager.h"
#include "Sound/SoundManager.h"
#include "FunctionLibrary.h"

UColorConfigurator::UColorConfigurator()
{
	// ビート演出用コンポーネントを生成
	BeatScaler = CreateDefaultSubobject<UBeatScalerComponent>(TEXT("BeatScaler"));
}

void UColorConfigurator::Init()
{
	// 各種初期化処理
	InitializeColorLogic();
	RegisterToColorManager();
	SetupMaterial();

	// サウンドマネージャを取得し、ビートイベントに登録
	const TObjectPtr<USoundManager> SoundManager = Cast<USoundManager>(GetLevelManager()->GetSoundManager().GetObject());
	if (!SoundManager) return;

	SoundManager->OnBeatDetected.AddDynamic(this, &UColorConfigurator::PlayBeatAnimation);
}

void UColorConfigurator::InitializeColorLogic()
{
	if (!ReactiveComponentClass) return;

	// 現在色を初期色に設定
	CurrentColor = StartColor;

	// 色リアクティブコンポーネントを生成
	ColorReactive = NewObject<UColorReactiveComponent>(this, ReactiveComponentClass);
	if (!ColorReactive) return;

	// 登録 & 有効化
	ColorReactive->RegisterComponent();
	ColorReactive->Activate(true);

	// 色エフェクトとナイアガラを初期化
	ColorReactive->InitColorEffectAndNiagara(StartColor, EffectType, NiagaraActors);
	ColorReactive->Init(bIsColorVariable);
}

void UColorConfigurator::RegisterToColorManager()
{
	// カラーマネージャに自身を登録
	if (UColorManager* ColorManager = GetColorManager())
	{
		ColorManager->RegisterTarget(TargetType, GetOwner());
	}
}

void UColorConfigurator::SetupMaterial()
{
	if (!bSetColor) return;

	// 初期色をカラーマネージャから取得
	StartColor = ALevelManager::GetInstance(GetWorld())
		->GetColorManager()
		->GetEffectColor(EffectType);

	// メッシュを取得して色を適用
	if (USkeletalMeshComponent* Mesh = GetStaticMesh())
	{
		Mesh->SetRenderCustomDepth(true);
		Mesh->SetCustomDepthStencilValue(10);

		if (UMaterialInstanceDynamic* DynMaterial = Mesh->CreateAndSetMaterialInstanceDynamic(0))
		{
			DynMaterial->SetVectorParameterValue(FName("BaseColor"), StartColor);
		}
	}
}

void UColorConfigurator::PlayBeatAnimation()
{
	// ビート演出が無効なら何もしない
	if (!bPlayBeatEffect) return;

	// ビート演出を実行
	if (BeatScaler)
	{
		BeatScaler->PlayBeat();
	}
}

void UColorConfigurator::ColorAction(FLinearColor NewColor, FEffectMatchResult MatchResult)
{
	if (!bEnableColorAction || !ColorReactive) return;

	// 色変数扱いなら材質に反映
	if (bIsColorVariable)
	{
		ApplyColorToMaterial(NewColor);
	}

	// 色一致判定を更新
	bIsColorMatch = ColorReactive->IsRGBDistancewithinThreshold(MatchResult, NewColor, bUseComplementaryColor);
}

void UColorConfigurator::SetColor(FLinearColor NewColor, FEffectMatchResult MatchResult)
{
	// 現在色を更新
	CurrentColor = NewColor;

	// 設定可能なら材質へ反映
	if (bSetColor)
	{
		ApplyColorToMaterial(CurrentColor);
	}

	// リアクティブコンポーネントへ色適用
	if (ColorReactive)
	{
		ColorReactive->InitColorEffectAndNiagara(CurrentColor, MatchResult.ClosestEffect, NiagaraActors);
	}

	// カラーマネージャが存在するならアクション実行
	if (const UColorManager* ColorManager = GetColorManager())
	{
		ColorAction(ColorManager->GetWorldColor(), MatchResult);
	}
}

void UColorConfigurator::ResetColor(FEffectMatchResult MatchResult)
{
	// 初期色にリセット
	SetColor(StartColor, MatchResult);
}

void UColorConfigurator::SetCurrentColor(FLinearColor NewColor)
{
	// 内部的に色だけ更新
	CurrentColor = NewColor;
}

void UColorConfigurator::SetColorMatch(bool bInColorMatch)
{
	// 色一致フラグを更新
	bIsColorMatch = bInColorMatch;
}

void UColorConfigurator::SetSelectMode(bool bInIsSelected)
{
	// 選択状態を更新
	bIsSelected = bInIsSelected;

	// リアクティブ側にも伝える
	if (ColorReactive)
	{
		ColorReactive->SetSelectMode(bIsSelected);
	}
}

bool UColorConfigurator::IsColorChange() const
{
	// 初期色と一致しているかを確認
	return ColorReactive && ColorReactive->IsColorMatch(StartColor);
}

bool UColorConfigurator::IsColorChange(FLinearColor Color) const
{
	// 指定色と一致しているかを確認
	return ColorReactive && ColorReactive->IsColorMatch(Color);
}

bool UColorConfigurator::CheckColorMatch(FEffectMatchResult MatchResult, const FLinearColor& FilterColor, bool bUseComplementary) const
{
	// マッチ判定を実行
	return ColorReactive && ColorReactive->IsRGBDistancewithinThreshold(MatchResult, FilterColor, bUseComplementary);
}

bool UColorConfigurator::IsColorMatch() const
{
	// 内部フラグによる一致判定
	return bIsColorMatch;
}

bool UColorConfigurator::IsColorMatch(const FLinearColor& FilterColor, const FLinearColor& TargetColor, float Tolerance) const
{
	// 2色間の一致確認
	return ColorReactive && ColorReactive->IsColorMatch(FilterColor, TargetColor, Tolerance);
}

bool UColorConfigurator::IsColorMatch(const FLinearColor& FilterColor, float Tolerance) const
{
	// 色と現在の色の一致確認
	return ColorReactive && ColorReactive->IsColorMatch(FilterColor, Tolerance);
}

bool UColorConfigurator::IsChangeable() const
{
	// 色変更可能フラグを返す
	return bColorChangeable;
}

bool UColorConfigurator::IsHidden() const
{
	// 非表示判定
	return ColorReactive && ColorReactive->IsHidden();
}

void UColorConfigurator::ApplyColorToMaterial(FLinearColor InColor)
{
	// 材質に色を適用
	if (ColorReactive)
	{
		ColorReactive->ApplyColorToMaterial(InColor);
	}
}

// =======================
// 補助関数（共通処理）
// =======================

USkeletalMeshComponent* UColorConfigurator::GetStaticMesh() const
{
	// "Mesh" という名前の SkeletalMeshComponent を探す
	return UFunctionLibrary::FindComponentByName<USkeletalMeshComponent>(GetOwner(), TEXT("Mesh"));
}

ALevelManager* UColorConfigurator::GetLevelManager() const
{
	// レベルマネージャ取得
	return ALevelManager::GetInstance(GetWorld());
}

UColorManager* UColorConfigurator::GetColorManager() const
{
	// カラーマネージャ取得
	const ALevelManager* LevelManager = GetLevelManager();
	return LevelManager ? LevelManager->GetColorManager() : nullptr;
}
