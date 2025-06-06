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
}

void AColorReactiveObject::BeginPlay()
{
	Super::BeginPlay();

	if (ReactiveComponentClass)
	{
		ColorReactiveComponent = NewObject<UColorReactiveComponent>(this, ReactiveComponentClass);
		if (ColorReactiveComponent)
		{
			ColorReactiveComponent->RegisterComponent(); // コンポーネントとして機能させるため必須
			ColorReactiveComponent->Activate(true);
		}
	}

	// ColorManager に登録
	ALevelManager::GetInstance(GetWorld())->GetColorManager()->RegisterTarget(ColorTargetType, this);

	ColorReactiveComponent->SetMyColor(Color);

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
	ColorReactiveComponent->Init(mesh);
}


void AColorReactiveObject::ColorAction(FLinearColor NewColor)
{
	if (!ColorReactiveComponent)
		return;

	ColorReactiveComponent->CheckColorMatch(NewColor);
}



