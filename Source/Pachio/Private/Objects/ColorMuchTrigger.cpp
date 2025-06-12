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
			ColorManager->RegisterTarget(EColorTargetType::Responders, this);
		}
	}
}

void AColorMuchTrigger::Init()
{
	InitializeColorLogic();
	RegisterToColorManager();
}

void AColorMuchTrigger::ColorAction(FLinearColor InColor)
{
	ApplyColorToMaterial(InColor);
	if (!ColorReactiveComponent)
		return;
	bColorMuch = ColorReactiveComponent->CheckColorMatch(InColor);
}

void AColorMuchTrigger::ApplyColorToMaterial(FLinearColor InColor)
{
	UStaticMeshComponent* Mesh = UFunctionLibrary::FindComponentByName<UStaticMeshComponent>(this, TEXT("StaticMesh"));
	if (!Mesh) return;

	UMaterialInstanceDynamic* DynMaterial = Mesh->CreateAndSetMaterialInstanceDynamic(0);
	if (!DynMaterial) return;

	DynMaterial->SetVectorParameterValue(FName("BaseColor"), InColor);
}