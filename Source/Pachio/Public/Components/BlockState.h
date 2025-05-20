// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BlockState.generated.h"

struct FAttackData;
class ABaseBlock;
class UMaterialInterface;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PACHIO_API UBlockState : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UBlockState();

	virtual bool OnEnter(ABaseBlock*, UWorld*,const FString materialID = "None");
	virtual bool OnUpdate(ABaseBlock*);
	virtual bool OnExit(ABaseBlock*);
	virtual bool OnHit(FAttackData,const FVector = FVector(0,0,0),const AActor* = nullptr);
protected:
	UPROPERTY()
	ABaseBlock* mOwner;
	UPROPERTY()
	UWorld* pWorld;
	UPROPERTY(EditAnywhere)
	FString MaterialID;
	int count;
};
