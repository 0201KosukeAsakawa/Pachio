// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/ColorReactiveSwitch.h"
#include "Components/ColorConfigurator.h"
#include "Manager/LevelManager.h"
#include "Manager/ColorManager.h"
#include "Components/ColorReactiveComponent.h"
#include "Components/BoxComponent.h"

AColorReactiveSwitch::AColorReactiveSwitch()
{
	// Box Component ��쐬
	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision"));
	BoxComponent->SetupAttachment(RootComponent); // �A�N�^�[�̃��[�g�R���|�[�l���g�ɐݒ�

	// �I�[�o�[���b�v�C�x���g�̃o�C���h
	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AColorReactiveSwitch::OnOverlapBegin);
}

void AColorReactiveSwitch::Init()
{
	AColorReactiveObject::Init();
}

void AColorReactiveSwitch::ColorAction(const FLinearColor InColor)
{
	if (!ColorConfigurator)
		return;
	AColorReactiveObject::ColorAction(InColor);
	ColorConfigurator->SetColor(InColor);
	if (ColorConfigurator->CheckColorMuch(InColor))
	{
		ALevelManager* levelManager = ALevelManager::GetInstance(GetWorld());
		if (levelManager == nullptr)
			return;
		if (levelManager->GetColorManager() == nullptr)
			return;

		levelManager->GetColorManager()->ColorEvent(ColorConfigurator->GetColorEventID(), InColor);
	}

}

void AColorReactiveSwitch::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (ColorConfigurator == nullptr)
		return;

	if (!OtherActor)
		return;

	if (!OtherActor->ActorHasTag("Player"))
		return;
	ALevelManager* levelManager = ALevelManager::GetInstance(GetWorld());
	if (levelManager == nullptr)
		return;
	if (levelManager->GetColorManager() == nullptr)
		return;

	levelManager->GetColorManager()->ColorEvent(ColorConfigurator->GetColorEventID(), ColorConfigurator->GetCurrentColor());
}
