// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/BlockState/IdleBlockState.h"
#include "Attack/AttackStrategy.h"

bool UIdleBlockState::OnEnter(AActor* owner, UWorld*)
{
	if (!owner)
		return false;

	mOwner = owner;
	count = 1;

	return true;
}

bool UIdleBlockState::OnUpdate(AActor*)
{
	if (!mOwner)
		return false;

	if (count > 0)
		return true;
	mOwner->Destroy();
	return true;
}


bool UIdleBlockState::OnExit(AActor*)
{
	return true;
}

bool UIdleBlockState::OnHit(FVector , FAttackData attackData)
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

