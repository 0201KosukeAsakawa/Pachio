// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/Color/ColorReactiveSwitch.h"
#include "Components/Color/ColorConfigurator.h"
#include "Manager/LevelManager.h"
#include "Manager/ColorManager.h"
#include "Components/Color/ColorReactiveComponent.h"
#include "Components/BoxComponent.h"

AColorReactiveSwitch::AColorReactiveSwitch()
{
	// Box Component ��쐬
	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision"));
	BoxComponent->SetupAttachment(RootComponent); // �A�N�^�[�̃��[�g�R���|�[�l���g�ɐݒ�
}

void AColorReactiveSwitch::Init()
{
	AColorReactiveObject::Init();
	SecondColor = ALevelManager::GetInstance(GetWorld())->GetColorManager()->GetEffectColor(Second);
}

void AColorReactiveSwitch::ColorAction(const FLinearColor InColor, FEffectMatchResult result)
{
	if (!ColorConfigurator)
		return;
	AColorReactiveObject::ColorAction(InColor, result);

	if (ColorConfigurator->CheckColorMatch(result, InColor))
	{
		ColorConfigurator->ApplyColorToMaterial(InColor);
		ALevelManager* levelManager = ALevelManager::GetInstance(GetWorld());
		if (levelManager == nullptr)
			return;
		if (levelManager->GetColorManager() == nullptr)
			return;

		levelManager->GetColorManager()->ColorEvent(ColorConfigurator->GetColorEventID(), InColor);
	}
	else if (ColorConfigurator->IsColorMatch(SecondColor, InColor))
	{
		ColorConfigurator->ApplyColorToMaterial(InColor);
		ALevelManager* levelManager = ALevelManager::GetInstance(GetWorld());
		if (levelManager == nullptr)
			return;
		if (levelManager->GetColorManager() == nullptr)
			return;

		levelManager->GetColorManager()->ColorEvent(ColorConfigurator->GetColorEventID(), InColor);
	}
}