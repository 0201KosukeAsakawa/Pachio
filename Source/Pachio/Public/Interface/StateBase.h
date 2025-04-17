// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GameFramework/Actor.h"
#include "StateBase.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UStateBase : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PACHIO_API IStateBase
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	
	virtual bool OnEnter(AActor*,UWorld*);
	virtual bool OnUpdate(AActor*, UWorld*);
	virtual bool OnExit(AActor*, UWorld*);
};
