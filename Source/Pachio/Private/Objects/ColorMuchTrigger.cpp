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
	AColorReactiveObject::Init();
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

	UMaterialInstanceDynamic* DynMaterial = Mesh->CreateAndSetMaterialInstanceDynamic(0);
	if (DynMaterial == nullptr)
		return;

	DynMaterial->SetVectorParameterValue(FName("BaseColor"), StartColor);
}
