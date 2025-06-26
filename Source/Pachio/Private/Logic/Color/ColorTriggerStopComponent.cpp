// Fill out your copyright notice in the Description page of Project Settings.


#include "Logic/Color/ColorTriggerStopComponent.h"
#include "FunctionLibrary.h"
#include "Components/BoxComponent.h"



// Sets default values for this component's properties
UColorTriggerStopComponent::UColorTriggerStopComponent()
{

}

void UColorTriggerStopComponent::OnColorMatched(const FLinearColor& FilterColor)
{
    UBoxComponent* box = UFunctionLibrary::FindComponentByName<UBoxComponent>(GetOwner(), TEXT("Collision"));
    UStaticMeshComponent* mesh = UFunctionLibrary::FindComponentByName<UStaticMeshComponent>(GetOwner(), TEXT("StaticMesh"));
    if (box != nullptr)
    {
        box->SetHiddenInGame(true);
        box->SetCollisionEnabled(ECollisionEnabled::NoCollision); 
    }
    if (mesh != nullptr)
    {
        mesh->SetVisibility(false);
        mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        mesh->SetCastShadow(false);
    }
}

void UColorTriggerStopComponent::OnColorMismatched(const FLinearColor& FilterColor)
{
    UBoxComponent* box = UFunctionLibrary::FindComponentByName<UBoxComponent>(GetOwner(), TEXT("Collision"));
    UStaticMeshComponent* mesh = UFunctionLibrary::FindComponentByName<UStaticMeshComponent>(GetOwner(), TEXT("StaticMesh"));
    if (box != nullptr)
    {
        box->SetHiddenInGame(false);
        box->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    }
    if (mesh != nullptr)
    {
        mesh->SetVisibility(true);
        mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
        mesh->SetCastShadow(true);
    }
}

