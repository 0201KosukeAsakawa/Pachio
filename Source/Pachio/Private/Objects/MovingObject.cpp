// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/MovingObject.h"

// Sets default values
AMovingObject::AMovingObject()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void AMovingObject::Init()
{
	InitializeColorLogic();
	RegisterToColorManager();
}

void AMovingObject::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (bIsMoving)
    {
        FVector CurrentLocation = GetActorLocation();
        FVector Direction = (OffLocation - CurrentLocation).GetSafeNormal();
        float Speed = 400.f; // 速度

        FVector NewLocation = CurrentLocation + Direction * Speed * DeltaTime;

        // 目的地に十分近ければ停止
        if (FVector::Dist(NewLocation, OffLocation) < 10.f)
        {
            NewLocation = OffLocation;
            bIsMoving = false;
        }

        SetActorLocation(NewLocation);
    }
}

void AMovingObject::ColorAction(FLinearColor InColor)
{
	bIsMoving = true;
}
