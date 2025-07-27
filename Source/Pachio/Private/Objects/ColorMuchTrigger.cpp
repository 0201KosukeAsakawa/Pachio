// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/ColorMuchTrigger.h"
#include "Manager/LevelManager.h"
#include "Manager/ColorManager.h"
#include "Components/ColorReactiveComponent.h"
#include "FunctionLibrary.h"

AColorMuchTrigger::AColorMuchTrigger()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AColorMuchTrigger::BeginPlay()
{
	Super::BeginPlay();
	if (ALevelManager* LevelManager = ALevelManager::GetInstance(GetWorld()))
	{
		if (UColorManager* ColorManager = LevelManager->GetColorManager())
		{
			ColorManager->RegisterTarget(ColorTargetType, this);
		}
	}
}

void AColorMuchTrigger::Init()
{
	InitializeColorLogic();    // �F�����R���|�[�l���g�̐����E�ݒ�
	RegisterToColorManager(); // �J���[�}�l�[�W���[�ւ̓o�^
	SetupMaterial();          // �}�e���A���ƃX�e���V���l�̐ݒ�
}

void AColorMuchTrigger::ColorAction(FLinearColor InColor)
{
	if (!ColorReactiveComponent)
		return;
	bColorMuch = ColorReactiveComponent->CheckColorMatch(InColor);
	if (!bColorMuch)
		return;
	ALevelManager* levelManager = ALevelManager::GetInstance(GetWorld());
	if (levelManager == nullptr)
		return;
	if (levelManager->GetColorManager() == nullptr)
		return;

 	levelManager->GetColorManager()->ColorEvent(EventID);
}

void AColorMuchTrigger::SetupMaterial()
{
	UStaticMeshComponent* Mesh = UFunctionLibrary::FindComponentByName<UStaticMeshComponent>(this, TEXT("StaticMesh"));
	if (Mesh == nullptr)
		return;

	Mesh->SetRenderCustomDepth(true);
	Mesh->SetCustomDepthStencilValue(10);
}


