// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/BlockState/EmptyState.h"
#include "Components/StaticMeshComponent.h"
#include "DataContainer/BlockDataContainer.h"
#include "FunctionLibrary.h"

bool UEmptyState::OnEnter(ABaseBlock* owner, UWorld* world, UBlockDataContainer* c,  FString materialID)
{
    if (!owner || !c)
        return false;
    mOwner = owner;

    pWorld = world;

    // �A�N�^�[�ɃA�^�b�`����Ă���S�ẴR���|�[�l���g��擾
    UStaticMeshComponent* MeshComp = UFunctionLibrary::FindComponentByName<UStaticMeshComponent>(mOwner, "StaticMesh");

    if (materialID == "None")
        materialID = MaterialID;

    UMaterialInterface* newMaterial = c->CreateMaterial(world, materialID);
    if (MeshComp && newMaterial)
    {
        MeshComp->SetMaterial(0, newMaterial);
    }
    Container = c;
    return true;
}
