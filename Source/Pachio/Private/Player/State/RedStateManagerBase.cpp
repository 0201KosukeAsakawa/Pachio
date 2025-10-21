// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/State/RedStateManagerBase.h"
#include "GameFramework/Character.h"

// Sets default values for this component's properties
URedStateManagerBase::URedStateManagerBase()
{

}

void URedStateManagerBase::Init_Implementation(APawn* Owner, UWorld* World)
{
	UStateManagerBase::Init_Implementation(Owner, World);
}

void URedStateManagerBase::Update_Implementation(float DeltaTime)
{

}
