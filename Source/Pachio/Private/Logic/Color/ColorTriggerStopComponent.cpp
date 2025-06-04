// Fill out your copyright notice in the Description page of Project Settings.


#include "Logic/Color/ColorTriggerStopComponent.h"
#include "FunctionLibrary.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"

// Sets default values for this component's properties
UColorTriggerStopComponent::UColorTriggerStopComponent()
{

}

void UColorTriggerStopComponent::OnColorMatched()
{
	UBoxComponent* box = UFunctionLibrary::FindComponentByName<UBoxComponent>(GetOwner(), TEXT("Collision"));
	UStaticMeshComponent* mesh = UFunctionLibrary::FindComponentByName<UStaticMeshComponent>(GetOwner(), TEXT("StaticMesh"));
	if (!box || !mesh)
		return;
	
	box->SetHiddenInGame(true);
	mesh->SetVisibility(false);
	mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	mesh->SetCastShadow(false);

}
