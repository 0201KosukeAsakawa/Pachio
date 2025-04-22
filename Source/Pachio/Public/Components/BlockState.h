// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BlockState.generated.h"

struct FAttackData;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PACHIO_API UBlockState : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UBlockState();

	virtual bool OnEnter(AActor*,UWorld*);
	virtual bool OnUpdate(AActor*);
	virtual bool OnExit(AActor*);
	virtual bool OnHit(FVector, FAttackData);
protected:
	UPROPERTY()
	AActor* mOwner;
	UPROPERTY()
	UWorld* pWorld;
	int count;
};
