// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/Color/ColorReactiveSwitch.h"
#include "Components/Color/ColorConfigurator.h"
#include "Manager/LevelManager.h"
#include "Manager/ColorManager.h"
#include "Components/Color/ColorReactiveComponent.h"
#include "Components/BoxComponent.h"

// =======================
// コンストラクタ
// =======================

AColorReactiveSwitch::AColorReactiveSwitch()
{
	// 当たり判定用の BoxComponent を生成して Root にアタッチ
	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision"));
	BoxComponent->SetupAttachment(RootComponent);
}

// =======================
// 初期化処理
// =======================

void AColorReactiveSwitch::Init()
{
	// 親クラスの初期化処理
	AColorReactiveObject::Init();

	// 2色目の判定用カラーを取得
	SecondColor = ALevelManager::GetInstance(GetWorld())
		->GetColorManager()
		->GetEffectColor(Second);
}

// =======================
// 色反応処理
// =======================

void AColorReactiveSwitch::ColorAction(const FLinearColor InColor, FEffectMatchResult result)
{
	// ColorConfigurator が存在しなければ処理中断
	if (!ColorConfigurator)
		return;

	// 親クラスの色処理を実行
	AColorReactiveObject::ColorAction(InColor, result);

	// -----------------------
	// 第一色との一致チェック
	// -----------------------
	if (ColorConfigurator->CheckColorMatch(result, InColor))
	{
		// 一致した色をマテリアルに反映
		ColorConfigurator->ApplyColorToMaterial(InColor);

		// LevelManager を取得
		ALevelManager* levelManager = ALevelManager::GetInstance(GetWorld());
		if (!levelManager || !levelManager->GetColorManager())
			return;

		// ColorEvent を発火（イベントIDで識別）
		levelManager->GetColorManager()->ColorEvent(ColorConfigurator->GetColorEventID(), InColor);
	}
	// -----------------------
	// 第二色との一致チェック
	// -----------------------
	else if (ColorConfigurator->IsColorMatch(SecondColor, InColor))
	{
		// 一致した色をマテリアルに反映
		ColorConfigurator->ApplyColorToMaterial(InColor);

		// LevelManager を取得
		ALevelManager* levelManager = ALevelManager::GetInstance(GetWorld());
		if (!levelManager || !levelManager->GetColorManager())
			return;

		// 第二色でも同様に ColorEvent を発火
		levelManager->GetColorManager()->ColorEvent(ColorConfigurator->GetColorEventID(), InColor);
	}
}
