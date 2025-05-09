// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/BlockState/EmptyState.h"
#include "Components/StaticMeshComponent.h"
#include "DataContainer/BlockDataContainer.h"
#include "FunctionLibrary.h"
#include "Manager/LevelManager.h"

bool UEmptyState::OnEnter(ABaseBlock* owner, UWorld* world,  FString materialID)
{
    if (!owner)
        return false;
    mOwner = owner;

    pWorld = world;


    UStaticMeshComponent* MeshComp = UFunctionLibrary::FindComponentByName<UStaticMeshComponent>(mOwner, "StaticMesh");

    if (materialID == "None")
        materialID = MaterialID;

    UMaterialInterface* newMaterial = ALevelManager::GetInstance(pWorld)->GetBlockContainer()->CreateMaterial(world, materialID);
    if (MeshComp && newMaterial)
    {
        MeshComp->SetMaterial(0, newMaterial);
    }
    return true;
}
