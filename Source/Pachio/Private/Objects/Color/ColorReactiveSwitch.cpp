// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/Color/ColorReactiveSwitch.h"
#include "Components/Color/ObjectColorComponent.h"
#include "Manager/LevelManager.h"
#include "Manager/ColorManager.h"
#include "Components/Color/ColorReactiveComponent.h"
#include "Components/BoxComponent.h"

// =======================
// コンストラクタ
// =======================

UColorReactiveSwitchComponent::UColorReactiveSwitchComponent()
{
	
}

// =======================
// 初期化処理
// =======================

void UColorReactiveSwitchComponent::Initialize()
{
	// 親クラスの初期化処理
	UObjectColorComponent::Initialize();
}

// =======================
// 色反応処理
// =======================

void UColorReactiveSwitchComponent::ActivateDirect(const FLinearColor& InColor)
{
	// -----------------------
	// 第一色との一致チェック
	// -----------------------
	if (UColorUtilityLibrary::IsHueSimilar(GetCurrentColor(), InColor))
	{
		// LevelManager を取得
		ALevelManager* levelManager = ALevelManager::GetInstance(GetWorld());
		if (!levelManager || !levelManager->GetColorManager())
			return;

		// ColorEvent を発火（イベントIDで識別）
		levelManager->GetColorManager()->ColorEvent(TargetEventID, InColor);
	}
}