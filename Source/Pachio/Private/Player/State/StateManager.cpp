// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/State/StateManager.h"
#include "Components/PlayerStateBase.h"
#include "Player/State/PlayerDefaultState.h"
#include "Player/State/PlayerSuperState.h"

// Sets default values for this component's properties
UStateManager::UStateManager()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UStateManager::Init(ACharacter* owner,UWorld* world)
{
	if (!owner || !world)
		return;

	mOwner = owner;
	pWorld = world;

	UPlayerDefaultState* Default = NewObject<UPlayerDefaultState>(mOwner);
	UPlayerSuperState* Super = NewObject<UPlayerSuperState>(mOwner);

	StateMap.Add("Default", Default);
	StateMap.Add("Super", Super);
	//StateMap.Add("Fire");

	ChangeState("Default");
	
}


// Called every frame
void UStateManager::Update(float deltaTime)
{
	if (CurrentState != nullptr)
	{
		CurrentState->OnUpdate(deltaTime);
	}
}

void UStateManager::ChangeState(FString nextState)
{
	UPlayerStateBase* next = StateMap[nextState];
	if (!next || !mOwner || !pWorld)
		return;

	if (CurrentState)
		CurrentState->OnExit(mOwner);


	CurrentState = next;
	CurrentState->OnEnter(mOwner,pWorld);
}

void UStateManager::Movement(const FInputActionValue& Value)
{
	if (CurrentState != nullptr)
	{
		CurrentState->Movement(Value);
	}
}