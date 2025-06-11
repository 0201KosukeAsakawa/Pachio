// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/ColorReactiveObject.h"
#include "Components/ColorReactiveComponent.h"
#include "Manager/LevelManager.h"
#include "FunctionLibrary.h"
#include "Manager/ColorManager.h"

AColorReactiveObject::AColorReactiveObject()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AColorReactiveObject::BeginPlay()
{
	Super::BeginPlay();
	InitializeColorLogic();
	RegisterToColorManager();
	SetupMaterial();
}

void AColorReactiveObject::InitializeColorLogic()
{
	if (ReactiveComponentClass)
	{
		ColorReactiveComponent = NewObject<UColorReactiveComponent>(this, ReactiveComponentClass);
		if (ColorReactiveComponent)
		{
			ColorReactiveComponent->RegisterComponent();
			ColorReactiveComponent->Activate(true);
			ColorReactiveComponent->SetMyColor(Color);

			UStaticMeshComponent* Mesh = UFunctionLibrary::FindComponentByName<UStaticMeshComponent>(this, TEXT("StaticMesh"));
			ColorReactiveComponent->Init(Mesh);
		}
	}
}

void AColorReactiveObject::RegisterToColorManager()
{
	if (ALevelManager* LevelManager = ALevelManager::GetInstance(GetWorld()))
	{
		if (UColorManager* ColorManager = LevelManager->GetColorManager())
		{
			ColorManager->RegisterTarget(ColorTargetType, this);
		}
	}
}

void AColorReactiveObject::SetupMaterial()
{
	UStaticMeshComponent* Mesh = UFunctionLibrary::FindComponentByName<UStaticMeshComponent>(this, TEXT("StaticMesh"));
	if (!Mesh) return;

	Mesh->SetRenderCustomDepth(true);
	Mesh->SetCustomDepthStencilValue(10);

	UMaterialInstanceDynamic* DynMaterial = Mesh->CreateAndSetMaterialInstanceDynamic(0);
	if (DynMaterial)
	{
		DynMaterial->SetVectorParameterValue(FName("BaseColor"), Color);
	}
}



void AColorReactiveObject::ColorAction(FLinearColor NewColor)
{
	if (!ColorReactiveComponent || bColorLock) return;

	bColorMuch = ColorReactiveComponent->CheckColorMatch(NewColor);
}
