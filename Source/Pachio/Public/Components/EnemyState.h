// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "EnemyState.generated.h"

class AEnemyCharacter;
class UEnemyStateComponent;

UCLASS()
class PACHIO_API UEnemyState : public UObject
{
	GENERATED_BODY()
	
public:
	virtual bool OnEnter(AEnemyCharacter* owner, UWorld* world , UEnemyStateComponent* LogicComponet);
	virtual bool OnUpdate(float deltaTime);
	virtual bool OnExit();
	virtual bool OnOverlap(AActor*);
protected:
	UPROPERTY()
	AEnemyCharacter* mOwner;
};
