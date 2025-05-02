// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/BlockState/BlockDepletedState.h"
#include "Attack/AttackStrategy.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInterface.h"
#include "FunctionLibrary.h"
#include "Objects/BaseBlock.h"
#include "DataContainer/BlockDataContainer.h"

bool UBlockDepletedState::OnEnter(ABaseBlock* owner, UWorld* world, UBlockDataContainer* c, FString materialID)
{
    if (!owner || !c)
        return false;
    mOwner = owner;

    // アクターにアタッチされている全てのコンポーネントを取得
    UStaticMeshComponent* MeshComp = UFunctionLibrary::FindComponentByName<UStaticMeshComponent>(mOwner, "StaticMesh");
    pWorld = world;

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

bool UBlockDepletedState::OnUpdate(ABaseBlock*)
{
    return true;
}
bool UBlockDepletedState::OnExit(ABaseBlock*)
{
    return true;
}
bool UBlockDepletedState::OnHit(FAttackData , FVector)
{
    mOwner->Destroy();
    return true;
}
