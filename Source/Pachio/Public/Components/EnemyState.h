// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "EnemyState.generated.h"

class AEnemyCharacter;

UCLASS()
class PACHIO_API UEnemyState : public UObject
{
	GENERATED_BODY()
	
public:
	virtual bool OnEnter(AEnemyCharacter* owner, UWorld* world);
	virtual bool OnUpdate(float deltaTime);
	virtual bool OnExit();

protected:
	UPROPERTY()
	AEnemyCharacter* mOwner;
};
