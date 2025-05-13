// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/BlockState/QuestionBlockIdleState.h"
#include "Objects/BaseBlock.h"
#include "Attack/AttackStrategy.h"
#include "DataContainer/BlockDataContainer.h"
#include "DataContainer/ItemDataContainer.h"
#include "FunctionLibrary.h"
#include "Manager/LevelManager.h"

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

bool UQuestionBlockIdleState::OnHit(FAttackData, FVector)
{
	--count;
	if ( !mOwner)
		return false;

	UBlockState* nextState = ALevelManager::GetInstance(GetWorld())->GetBlockContainer()->CreateState(GetWorld(), "Empty");

	if (!nextState)
		return false;

	//FVector v = (OtherActor->GetActorLocation() - mOwner->GetActorLocation()).GetSafeNormal();
	//FVector OwnerRight = mOwner->GetActorRightVector(); // ローカル座標系のY軸（右方向）
	//float YComponent = FVector::DotProduct(v, OwnerRight); // -1〜1の範囲で、右方向への成分

	//TODO:引数の修正もとむ　
	ALevelManager::GetInstance(GetWorld())->GetItemContainer()->GenerateItem(mOwner->GetDropItemID(), mOwner->GetActorLocation() + FVector(0, 5, 0), FVector(0, /*YComponent*/1, 0), 5.0f, FVector(0, 0, 1));

	mOwner->ChangeState(nextState);



	return true;
}

