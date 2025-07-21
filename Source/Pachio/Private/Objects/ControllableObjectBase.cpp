// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/ControllableObjectBase.h"
#include "Components/PlayerInputComponent.h"

// Sets default values
AControllableObjectBase::AControllableObjectBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AControllableObjectBase::BeginPlay()
{
	Super::BeginPlay();
	UPlayerInputComponent* PlayerInputData = GetComponentByClass<UPlayerInputComponent>();
	if (PlayerInputData)
	{
		PlayerInputData->Init(Controller);
	}
}

// Called every frame
void AControllableObjectBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

