// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/BlockState/QuestionBlockIdleState.h"
#include "Objects/BaseBlock.h"
#include "Attack/AttackStrategy.h"
#include "DataContainer/BlockDataContainer.h"
#include "DataContainer/ItemDataContainer.h"
#include "FunctionLibrary.h"
#include "Manager/LevelManager.h"
#include "Interface/StateControllable.h"
#include "Player/State/PlayerDefaultState.h"

bool UQuestionBlockIdleState::OnEnter(ABaseBlock* owner, UWorld* world,  FString materialName)
{
	if (!owner)
		return false;

	mOwner = owner;
	count = 1;
	pWorld = world;


	UStaticMeshComponent* MeshComp = UFunctionLibrary::FindComponentByName<UStaticMeshComponent>(mOwner, "StaticMesh");

	if (materialName == "None")
		materialName = MaterialID;

	UMaterialInterface* newMaterial = ALevelManager::GetInstance(GetWorld())->GetBlockContainer()->CreateMaterial(world, materialName);
	if (MeshComp && newMaterial)
	{
		MeshComp->SetMaterial(0, newMaterial);
	}

	return true;
}

bool UQuestionBlockIdleState::OnUpdate(ABaseBlock*)
{

	return true;
}


bool UQuestionBlockIdleState::OnExit(ABaseBlock*)
{
	return true;
}

bool UQuestionBlockIdleState::OnHit(FAttackData, FVector , const AActor* hitActor)
{
	--count;
	if ( !mOwner)
		return false;

	UBlockState* nextState = ALevelManager::GetInstance(GetWorld())->GetBlockContainer()->CreateState(GetWorld(), "Empty");

	if (!nextState)
		return false;
	
	if (mOwner->GetDropItemID() != "PowerUP")
	{
		ALevelManager::GetInstance(GetWorld())->GetItemContainer()->GenerateItem(mOwner->GetDropItemID(), mOwner->GetActorLocation() + FVector(0, 5, 0), FVector(0, /*YComponent*/1, 0), 5.0f, FVector(0, 0, 1));
	}
	else
	{
		const IStateControllable* is = Cast<IStateControllable>(hitActor);
		if (!is)
			return false;

		if (Cast<UPlayerDefaultState>(is->GetPlayerState()))
		{
			ALevelManager::GetInstance(GetWorld())->GetItemContainer()->GenerateItem("SuperMush", mOwner->GetActorLocation() + FVector(0, 5, 0), FVector(0, /*YComponent*/1, 0), 5.0f, FVector(0, 0, 1));
		}
		else
			ALevelManager::GetInstance(GetWorld())->GetItemContainer()->GenerateItem("Flower", mOwner->GetActorLocation() + FVector(0, 5, 0), FVector(0, /*YComponent*/1, 0), 5.0f, FVector(0, 0, 1));
	}
	
	mOwner->ChangeState(nextState);



	return true;
}

