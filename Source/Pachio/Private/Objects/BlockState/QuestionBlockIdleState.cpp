// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/BlockState/QuestionBlockIdleState.h"
#include "Attack/AttackStrategy.h"
#include "DataContainer/BlockDataContainer.h"
#include "Objects/BaseBlock.h"
#include "FunctionLibrary.h"

bool UQuestionBlockIdleState::OnEnter(ABaseBlock* owner, UWorld* , UBlockDataContainer*c)
{
	if (!owner)
		return false;

	mOwner = owner;
	count = 1;
	Container = c;

	// �A�N�^�[�ɃA�^�b�`����Ă���S�ẴR���|�[�l���g��擾
	UStaticMeshComponent* MeshComp = UFunctionLibrary::FindComponentByName<UStaticMeshComponent>(mOwner, "StaticMesh");
	if (!NewMaterial)
	{
		// �V�����}�e���A����擾�i�Ⴆ�΁A�Q�[���̃A�Z�b�g����j
		NewMaterial = LoadObject<UMaterialInterface>(nullptr, TEXT("Material'/Game/Materials/NewMaterial.NewMaterial'"));
	}

	if (NewMaterial && MeshComp)
	{
		// �}�e���A����ύX
		MeshComp->SetMaterial(0, NewMaterial); // 0 �̓}�e���A���̃C���f�b�N�X
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
