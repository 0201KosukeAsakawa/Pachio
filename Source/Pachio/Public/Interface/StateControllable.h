// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Player/State/StateManagerBase.h"
#include "StateControllable.generated.h"

class UPlayerStateComponent;

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UStateControllable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PACHIO_API IStateControllable
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual UPlayerStateComponent* ChangeState(EPlayerStateType Tag);
	virtual UPlayerStateComponent* GetPlayerState()const;
};
