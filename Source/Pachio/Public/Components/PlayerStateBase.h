// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameFramework/Character.h"
#include "PlayerStateBase.generated.h"

struct FInputActionValue;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PACHIO_API UPlayerStateBase : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UPlayerStateBase();
	virtual bool OnEnter(ACharacter*, UWorld*);
	virtual bool OnUpdate(ACharacter*);
	virtual bool OnExit(ACharacter*);
	virtual bool OnSkill(const FInputActionValue&);
	virtual void Jump(const FInputActionValue& Value);
	virtual void Movement(const FInputActionValue& Value);
protected:
	UPROPERTY()
	ACharacter* mOwner;
	UPROPERTY()
	UWorld* pWorld;
	float mMoveSpeed;
};
