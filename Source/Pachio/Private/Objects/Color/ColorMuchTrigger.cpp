// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/Color/ColorMuchTrigger.h"
#include "Manager/LevelManager.h"
#include "Manager/ColorManager.h"
#include "Components/Color/ColorReactiveComponent.h"
#include "FunctionLibrary.h"

AColorMuchTrigger::AColorMuchTrigger()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AColorMuchTrigger::BeginPlay()
{
	Super::BeginPlay();
	
}

void AColorMuchTrigger::Init()
{
	
}

void AColorMuchTrigger::ColorAction(FLinearColor InColor, FEffectMatchResult)
{
	
}

void AColorMuchTrigger::SetupMaterial()
{
	
}


