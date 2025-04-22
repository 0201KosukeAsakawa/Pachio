// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/BlockState/IdleBlockState.h"
#include "Attack/AttackStrategy.h"

bool UQuestionBlockIdleState::OnEnter(AActor* owner, UWorld*)
{
	if (!owner)
		return false;

	mOwner = owner;
	count = 1;

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

