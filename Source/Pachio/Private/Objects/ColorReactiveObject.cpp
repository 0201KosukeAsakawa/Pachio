// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/ColorReactiveObject.h"
#include "Components/ColorReactiveComponent.h"
#include "Manager/LevelManager.h"
#include "FunctionLibrary.h"
#include "Manager/ColorManager.h"
#include "Logic/Color/ColorTriggerStopComponent.h"
#include "Logic/Color/ColorProximitySpawner.h"

// Sets default values
AColorReactiveObject::AColorReactiveObject()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	if (!ReactiveComponent)
	ReactiveComponent = CreateDefaultSubobject<UColorTriggerStopComponent>(TEXT("StopComponent"));
	check(ReactiveComponent != nullptr);
}

void AColorReactiveObject::BeginPlay()
{
	Super::BeginPlay();

	// ColorManager に登録
	ALevelManager::GetInstance(GetWorld())->GetColorManager()->RegisterTarget(ColorTargetType, this);
	ReactiveComponent->SetMyColor(Color);

	// StaticMeshComponent を取得
	UStaticMeshComponent* mesh = UFunctionLibrary::FindComponentByName<UStaticMeshComponent>(this, TEXT("StaticMesh"));
	if (!mesh)
		return;

	// Custom Depth を有効化
	mesh->SetRenderCustomDepth(true);
	mesh->SetCustomDepthStencilValue(10);

	// マテリアルの色を変更
	UMaterialInstanceDynamic* DynMaterial = mesh->CreateAndSetMaterialInstanceDynamic(0);
	if (DynMaterial)
	{
		DynMaterial->SetVectorParameterValue(FName("BaseColor"), Color);
	}
}


void AColorReactiveObject::ColorAction(FLinearColor NewColor)
{
	if (!ReactiveComponent)
		return;

	ReactiveComponent->CheckColorMatch(NewColor);
}



