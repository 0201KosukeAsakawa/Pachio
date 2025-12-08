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
	// 当たり判定用の BoxComponent を生成して Root にアタッチ
	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision"));
	
}

// =======================
// 初期化処理
// =======================

void UColorReactiveSwitchComponent::Initialize()
{
	// 親クラスの初期化処理
	UObjectColorComponent::Initialize();
	// 2色目の判定用カラーを取得
	SecondaryColor = ALevelManager::GetInstance(GetWorld())
		->GetColorManager()
		->GetEffectColor(SecondaryEffect);
}

// =======================
// 色反応処理
// =======================

void UColorReactiveSwitchComponent::ApplyColorWithMatching(const FLinearColor& InColor)
{
	// 親クラスの色処理を実行
	//AColorReactiveObject::ApplyColorWithMatching(InColor);

	// -----------------------
	// 第一色との一致チェック
	// -----------------------
	if (UColorUtilityLibrary::GetHueAngleDistance(GetCurrentColor(), InColor))
	{
		// 一致した色をマテリアルに反映
		SetColor(InColor);

		// LevelManager を取得
		ALevelManager* levelManager = ALevelManager::GetInstance(GetWorld());
		if (!levelManager || !levelManager->GetColorManager())
			return;

		// ColorEvent を発火（イベントIDで識別）
		levelManager->GetColorManager()->ColorEvent(GetColorEventID(), InColor);
	}
	// -----------------------
	// 第二色との一致チェック
	// -----------------------
	else if (UColorUtilityLibrary::IsHueSimilar(SecondaryColor, InColor))
	{
		// 一致した色をマテリアルに反映
		SetColor(InColor);

		// LevelManager を取得
		ALevelManager* levelManager = ALevelManager::GetInstance(GetWorld());
		if (!levelManager || !levelManager->GetColorManager())
			return;

		// 第二色でも同様に ColorEvent を発火
		levelManager->GetColorManager()->ColorEvent(GetColorEventID(), InColor);
	}
}

void UColorReactiveSwitchComponent::OnRegister()
{
	UObjectColorComponent::OnRegister();

	if (AActor* Owner = GetOwner())
	{
		if (USceneComponent* RootComp = Owner->GetRootComponent())
		{
			if (BoxComponent && BoxComponent->GetAttachParent() == nullptr)
			{
				BoxComponent->SetupAttachment(RootComp);
			}
		}
	}
}