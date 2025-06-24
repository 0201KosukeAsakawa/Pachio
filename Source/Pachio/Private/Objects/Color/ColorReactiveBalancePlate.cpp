// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/Color/ColorReactiveBalancePlate.h"
#include "Kismet/KismetMathLibrary.h"

AColorReactiveBalancePlate::AColorReactiveBalancePlate()
{
    PrimaryActorTick.bCanEverTick = true;
}

void AColorReactiveBalancePlate::BeginPlay()
{
    Super::BeginPlay();
    mDefaultPosition = GetActorLocation();
    v = mDefaultPosition;
}

void AColorReactiveBalancePlate::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AColorReactiveBalancePlate::SetLocation(FVector next)
{
    v = next;
    if (!IsReached(GetActorLocation(), v))
        SetActorLocation(UKismetMathLibrary::VLerp(GetActorLocation(), v, 0.1f));
    else
        v = GetActorLocation();
}

void AColorReactiveBalancePlate::ResetLocation()
{
    if (!IsReached(GetActorLocation(), mDefaultPosition))
        SetActorLocation(UKismetMathLibrary::VLerp(GetActorLocation(), mDefaultPosition, 0.05f));
    else
        v = GetActorLocation();
}

bool AColorReactiveBalancePlate::IsReached(FVector v1, FVector v2)
{
    return (v1 - v2).Length() <= 2.0f;
}