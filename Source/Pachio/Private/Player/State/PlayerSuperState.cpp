// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/State/PlayerSuperState.h"
#include "InputActionValue.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/StaticMeshComponent.h"

bool UPlayerSuperState::OnEnter(ACharacter* owner, UWorld* world)
{
	if (owner == nullptr || world == nullptr)
	{
		return false;
	}

	mOwner = owner;
	pWorld = world;

	if (NewMaterial != nullptr)
	{
		UStaticMeshComponent* StaticMeshComp = owner->FindComponentByClass<UStaticMeshComponent>();
		UMaterialInterface* N = NewMaterial.LoadSynchronous();
		if (N != nullptr)
		{
			StaticMeshComp->SetMaterial(0, N);
		}
	}

	mMoveSpeed = 100.0f;


	return true;
}

bool UPlayerSuperState::OnUpdate(float)
{
	return false;
}

bool UPlayerSuperState::OnExit(ACharacter*)
{
	return false;
}

bool UPlayerSuperState::OnSkill(const FInputActionValue&)
{
	return false;
}