// Fill out your copyright notice in the Description page of Project Settings.


#include "Logic/Color/ColorProximitySpawner.h"
#include "FunctionLibrary.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"

UColorProximitySpawner::UColorProximitySpawner()
{
}

void UColorProximitySpawner::OnColorMatched(const FLinearColor& FilterColor)
{
    UBoxComponent* box = UFunctionLibrary::FindComponentByName<UBoxComponent>(GetOwner(), TEXT("Collision"));
    UStaticMeshComponent* mesh = UFunctionLibrary::FindComponentByName<UStaticMeshComponent>(GetOwner(), TEXT("StaticMesh"));
    if (!box || !mesh)
        return;

    box->SetHiddenInGame(false);
    mesh->SetVisibility(true);
    mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    mesh->SetCastShadow(true);
}

void UColorProximitySpawner::OnColorMismatched(const FLinearColor& FilterColor)
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

void UColorProximitySpawner::ChangeColor(const FLinearColor& FilterColor)
{

}

