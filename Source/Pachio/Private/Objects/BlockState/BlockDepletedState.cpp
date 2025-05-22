// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/BlockState/BlockDepletedState.h"
#include "Objects/BaseBlock.h"
#include "Attack/AttackStrategy.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInterface.h"
#include "Manager/LevelManager.h"
#include "FunctionLibrary.h"

#include "DataContainer/BlockDataContainer.h"

bool UBlockDepletedState::OnEnter(ABaseBlock* owner, UWorld* world, FString materialID)
{
    if (!owner)
        return false;
    mOwner = owner;

    // アクターにアタッチされている全てのコンポーネントを取得
    UStaticMeshComponent* MeshComp = UFunctionLibrary::FindComponentByName<UStaticMeshComponent>(mOwner, "StaticMesh");
    pWorld = world;

    if (materialID == "None")
        materialID = MaterialID;

    UMaterialInterface* newMaterial = ALevelManager::GetInstance(pWorld)->GetBlockContainer()->CreateMaterial(world, materialID);
    if (MeshComp && newMaterial)
    {
        MeshComp->SetMaterial(0, newMaterial);
    }
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
bool UBlockDepletedState::OnHit(FAttackData , FVector , const AActor*)
{
    mOwner->Destroy();
    return true;
}
