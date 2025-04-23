// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/BlockState/QuestionBlockIdleState.h"
#include "Attack/AttackStrategy.h"

bool UQuestionBlockIdleState::OnEnter(AActor* owner, UWorld*)
{
	if (!owner)
		return false;

	mOwner = owner;
	count = 1;

	// アクターにアタッチされている全てのコンポーネントを取得
	UStaticMeshComponent* MeshComp = UFunctionLibrary::FindComponentByName<UStaticMeshComponent>(mOwner, "StaticMesh");
	if (!NewMaterial)
	{
		// 新しいマテリアルを取得（例えば、ゲームのアセットから）
		NewMaterial = LoadObject<UMaterialInterface>(nullptr, TEXT("Material'/Game/Materials/NewMaterial.NewMaterial'"));
	}

	if (NewMaterial && MeshComp)
	{
		// マテリアルを変更
		MeshComp->SetMaterial(0, NewMaterial); // 0 はマテリアルのインデックス
	}

	return true;
}

bool UQuestionBlockIdleState::OnUpdate(AActor*)
{
	if (!mOwner)
		return false;

	if (count > 0)
		return true;
	mOwner->Destroy();
	return true;
}


bool UQuestionBlockIdleState::OnExit(AActor*)
{
	return true;
}

bool UQuestionBlockIdleState::OnHit(FVector , FAttackData attackData)
{
	//if (attackData.attackType == EAttackType::Enemy)
	//	return false;

	//if (attackData.breakLevel == EBreakLevel::Functional)
	//{
	//	//ItemDataContainer->DropItem(FString,FVector); 
	//}

	//else(attackData.breakLevel == EBreakLevel::Breakable)
		//Destroy();

	--count;
	return true;
}

