// Fill out your copyright notice in the Description page of Project Settings.


#include "Logic/Event_Gimmick/ColorReactiveBalanceSystem.h"
#include "Components/Color/ColorReactiveComponent.h"
#include "Objects/Color/ColorReactiveBalancePlate.h"
#include "Kismet/KismetMathLibrary.h"

AColorReactiveBalanceSystem::AColorReactiveBalanceSystem()
{
    PrimaryActorTick.bCanEverTick = true;
}

void AColorReactiveBalanceSystem::Init()
{
    AColorReactiveObject::Init();
}

void AColorReactiveBalanceSystem::ColorAction(const FLinearColor InColor, FEffectMatchResult)
{

}


float AColorReactiveBalanceSystem::GetColorDistance(const FLinearColor& A, const FLinearColor& B) const
{
    return FVector(A.R - B.R, A.G - B.G, A.B - B.B).Length();
}