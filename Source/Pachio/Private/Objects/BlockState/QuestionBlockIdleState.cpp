// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/BlockState/QuestionBlockIdleState.h"
#include "Attack/AttackStrategy.h"
#include "DataContainer/BlockDataContainer.h"
#include "Objects/BaseBlock.h"
#include "FunctionLibrary.h"
#include "DataContainer/ItemDataContainer.h"
#include "Manager/LevelManager.h"

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

bool UQuestionBlockIdleState::OnUpperAttack(const AActor* OtherActor, FVector)
{
	--count;
	if (!OtherActor || !mOwner || !Container || !OtherActor->ActorHasTag("Player"))
		return false;

	UBlockState* nextState = Container->CreateState(GetWorld(), "Empty");

	if (!nextState)
		return false;

	FVector v = (OtherActor->GetActorLocation() - mOwner->GetActorLocation()).GetSafeNormal();
	FVector OwnerRight = mOwner->GetActorRightVector(); // ローカル座標系のY軸（右方向）
	float YComponent = FVector::DotProduct(v, OwnerRight); // -1〜1の範囲で、右方向への成分

		//TODO:引数の修正もとむ　
		ALevelManager::GetComponent(GetWorld())->GetItemContainer()->GenerateItem(mOwner->GetDropItemID(), mOwner->GetActorLocation() + FVector(0, 5, 0), FVector(0, /*YComponent*/1, 0), 5.0f, FVector(0, 0, 1));

	mOwner->ChangeState(nextState);



	return true;
}
