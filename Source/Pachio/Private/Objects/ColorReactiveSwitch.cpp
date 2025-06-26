// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/ColorReactiveSwitch.h"
#include "Manager/LevelManager.h"
#include "Manager/ColorManager.h"
#include "Components/ColorReactiveComponent.h"
#include "Components/BoxComponent.h"

AColorReactiveSwitch::AColorReactiveSwitch()
{
	// Box Component を作成
	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision"));
	BoxComponent->SetupAttachment(RootComponent); // アクターのルートコンポーネントに設定

	// オーバーラップイベントのバインド
	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AColorReactiveSwitch::OnOverlapBegin);
}

void AColorReactiveSwitch::Init()
{
	AColorReactiveObject::Init();
}

void AColorReactiveSwitch::ColorAction(const FLinearColor InColor)
{
	if (!ColorReactiveComponent)
		return;
	AColorReactiveObject::ColorAction(InColor);
	bColorMuch = ColorReactiveComponent->CheckColorMatch(InColor);
}

void AColorReactiveSwitch::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor)
		return;

	if (!OtherActor->ActorHasTag("Player"))
		return;
	ALevelManager* levelManager = ALevelManager::GetInstance(GetWorld());
	if (levelManager == nullptr)
		return;
	if (levelManager->GetColorManager() == nullptr)
		return;

	levelManager->GetColorManager()->ColorEvent(EventID);
}
