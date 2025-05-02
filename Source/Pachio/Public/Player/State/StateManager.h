// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include <EnhancedInputLibrary.h>
#include "StateManager.generated.h"


class UPlayerStateBase;
class Player;


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PACHIO_API UStateManager : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UStateManager();



public:
	// Called when the game starts
	 void Init(ACharacter*,UWorld*);

public:	
	// Called every frame
	void Update(float deltaTime);

	void ChangeState(FString nextState);

	void Movement(const FInputActionValue& Value);

private:
	TMap<FString, UPlayerStateBase*>StateMap;
	UPROPERTY()
	ACharacter* mOwner;
	UPROPERTY()
	UPlayerStateBase* CurrentState;
	UPROPERTY()
	UWorld* pWorld;
		
};
