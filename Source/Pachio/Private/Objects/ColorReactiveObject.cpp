// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/ColorReactiveObject.h"
#include "Components/ColorReactiveComponent.h"
#include "Manager/LevelManager.h"
#include "Manager/ColorManager.h"
#include "FunctionLibrary.h"


AColorReactiveObject::AColorReactiveObject()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AColorReactiveObject::BeginPlay()
{
	Super::BeginPlay();
	Init();
}

void AColorReactiveObject::Init()
{
	InitializeColorLogic();
	RegisterToColorManager();
	SetupMaterial();
}

void AColorReactiveObject::InitializeColorLogic()
{
	if (ReactiveComponentClass == nullptr)
		return;

	ColorReactiveComponent = NewObject<UColorReactiveComponent>(this, ReactiveComponentClass);
	if (ColorReactiveComponent == nullptr)
		return;

	ColorReactiveComponent->RegisterComponent();
	ColorReactiveComponent->Activate(true);
	ColorReactiveComponent->SetMyColor(Color);

	UStaticMeshComponent* Mesh = UFunctionLibrary::FindComponentByName<UStaticMeshComponent>(this, TEXT("StaticMesh"));
	if (Mesh == nullptr)
		return;
	ColorReactiveComponent->Init(Mesh);


}

void AColorReactiveObject::RegisterToColorManager()
{
	ALevelManager* LevelManager = ALevelManager::GetInstance(GetWorld());
	if (LevelManager == nullptr)
		return;
	UColorManager* ColorManager = LevelManager->GetColorManager();
	if (ColorManager == nullptr)
		return;

	ColorManager->RegisterTarget(ColorTargetType, this);

}

void AColorReactiveObject::SetupMaterial()
{
	UStaticMeshComponent* Mesh = UFunctionLibrary::FindComponentByName<UStaticMeshComponent>(this, TEXT("StaticMesh"));
	if (Mesh == nullptr)
		return;

	Mesh->SetRenderCustomDepth(true);
	Mesh->SetCustomDepthStencilValue(10);

	UMaterialInstanceDynamic* DynMaterial = Mesh->CreateAndSetMaterialInstanceDynamic(0);
	if (DynMaterial == nullptr)
		return;

	DynMaterial->SetVectorParameterValue(FName("BaseColor"), Color);

}



void AColorReactiveObject::ColorAction(FLinearColor NewColor)
{
	if (bColorLock ||ColorReactiveComponent == nullptr )
		return;

	bColorMuch = ColorReactiveComponent->CheckColorMatch(NewColor);
}


void AColorReactiveObject::ApplyColorToMaterial(FLinearColor InColor)
{
	UStaticMeshComponent* Mesh = UFunctionLibrary::FindComponentByName<UStaticMeshComponent>(this, TEXT("StaticMesh"));
	if (!Mesh) return;

	UMaterialInstanceDynamic* DynMaterial = Mesh->CreateAndSetMaterialInstanceDynamic(0);
	if (!DynMaterial) return;

	DynMaterial->SetVectorParameterValue(FName("BaseColor"), InColor);
}