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
    AActor* Owner = GetOwner();
    if (!Owner) return;

    Owner->SetActorHiddenInGame(false);
    Owner->SetActorTickEnabled(true);
    Owner->SetActorEnableCollision(true);

    TArray<UActorComponent*> Components = Owner->GetComponents().Array();
    for (UActorComponent* Comp : Components)
    {
        if (UPrimitiveComponent* Prim = Cast<UPrimitiveComponent>(Comp))
        {
            Prim->SetVisibility(true);
            Prim->SetHiddenInGame(false);
            Prim->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
            Prim->SetComponentTickEnabled(true);
            Prim->SetCastShadow(true);
        }
    }
}

void UColorProximitySpawner::OffMesh()
{
    AActor* Owner = GetOwner();
    if (!Owner) return;

    Owner->SetActorHiddenInGame(true);
    Owner->SetActorTickEnabled(false);
    Owner->SetActorEnableCollision(false);

    TArray<UActorComponent*> Components = Owner->GetComponents().Array();
    for (UActorComponent* Comp : Components)
    {
        if (UPrimitiveComponent* Prim = Cast<UPrimitiveComponent>(Comp))
        {
            Prim->SetVisibility(false);
            Prim->SetHiddenInGame(true);
            Prim->SetCollisionEnabled(ECollisionEnabled::NoCollision);
            Prim->SetComponentTickEnabled(false);
            Prim->SetCastShadow(false);
        }
    }
}
