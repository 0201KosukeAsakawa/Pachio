// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/BlockState/QuestionBlockIdleState.h"
#include "Attack/AttackStrategy.h"
#include "DataContainer/BlockDataContainer.h"
#include "Objects/BaseBlock.h"
#include "FunctionLibrary.h"

bool UQuestionBlockIdleState::OnEnter(ABaseBlock* owner, UWorld* world, UBlockDataContainer* c, FString materialName)
{
	if (!owner)
		return false;

	mOwner = owner;
	count = 1;
	Container = c;
	pWorld = world;

	// �A�N�^�[�ɃA�^�b�`����Ă���S�ẴR���|�[�l���g��擾
	UStaticMeshComponent* MeshComp = UFunctionLibrary::FindComponentByName<UStaticMeshComponent>(mOwner, "StaticMesh");

	if (materialName == "None")
		materialName = MaterialID;

	UMaterialInterface* newMaterial = c->CreateMaterial(world, materialName);
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

bool UQuestionBlockIdleState::OnHit(const AActor* OtherActor, FVector)
{
	--count;
	if (!OtherActor || !OtherActor->ActorHasTag("Player"))
		return false;

	if (!Container)
		return false;

	UBlockState* nextState = Container->CreateState(GetWorld(), "Enpty");

	if (!nextState)
		return false;

	mOwner->ChangeState(nextState);
	return true;
}
