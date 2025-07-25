// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/ActionControl/CharacterActionInterfaces.h"
#include "ControllableObjectBase.generated.h"
class UInputComponent;
class UMoveComponent;
UCLASS()
class PACHIO_API AControllableObjectBase :	public APawn , public IControllableAbility
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AControllableObjectBase();
	void SwitchControll(ACharacter* player);
	virtual void Action(const FInputActionValue& Value)override;
};
