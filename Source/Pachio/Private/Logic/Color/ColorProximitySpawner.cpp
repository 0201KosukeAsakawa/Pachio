// Fill out your copyright notice in the Description page of Project Settings.


#include "Logic/Color/ColorProximitySpawner.h"
#include "FunctionLibrary.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"

UColorProximitySpawner::UColorProximitySpawner()
{
    OffMesh();
}

void UColorProximitySpawner::OnColorMatched(const FLinearColor& FilterColor)
{
    OnMesh();
}

void UColorProximitySpawner::OnColorMismatched(const FLinearColor& FilterColor)
{
    OffMesh();
}

void UColorProximitySpawner::OnMesh()
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

void UColorProximitySpawner::OffMesh()
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

