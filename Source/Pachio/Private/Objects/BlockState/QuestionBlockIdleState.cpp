// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/BlockState/QuestionBlockIdleState.h"
#include "Objects/BaseBlock.h"
#include "Attack/AttackStrategy.h"
#include "DataContainer/BlockDataContainer.h"
#include "DataContainer/ItemDataContainer.h"
#include "FunctionLibrary.h"
#include "Manager/LevelManager.h"
#include "Manager/ScoreManager.h"
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
	return true;
}

