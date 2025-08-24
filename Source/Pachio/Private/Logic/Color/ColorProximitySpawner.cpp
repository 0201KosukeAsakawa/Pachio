// Fill out your copyright notice in the Description page of Project Settings.


#include "Logic/Color/ColorProximitySpawner.h"
#include "FunctionLibrary.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"

UColorProximitySpawner::UColorProximitySpawner()
{
    OffMesh();
}

bool  UColorProximitySpawner::OnColorMatched(const FLinearColor& FilterColor)
{
    OnMesh();
    return false;
}

bool UColorProximitySpawner::OnColorMismatched(const FLinearColor& FilterColor)
{
    OffMesh();
    return true;
}

void UColorProximitySpawner::OnMesh()
{
    AActor* Owner = GetOwner();
    if (!Owner) return;

    // �A�N�^�[�\�� & Tick �ĊJ
    Owner->SetActorHiddenInGame(false);
    Owner->SetActorTickEnabled(true);

    for (UActorComponent* Comp : Owner->GetComponents())
    {
        if (UPrimitiveComponent* Prim = Cast<UPrimitiveComponent>(Comp))
        {
            Prim->SetVisibility(true);
            Prim->SetHiddenInGame(false);
            Prim->SetCastShadow(true);
            Prim->SetComponentTickEnabled(true);
            Prim->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
        }
    }
}

void UColorProximitySpawner::OffMesh()
{
    AActor* Owner = GetOwner();
    if (!Owner) return;

    // �A�N�^�[��\�� & Tick ��~
    Owner->SetActorHiddenInGame(true);
    Owner->SetActorTickEnabled(false);

    // �R���|�[�l���g�𒲐�
    for (UActorComponent* Comp : Owner->GetComponents())
    {
        if (UPrimitiveComponent* Prim = Cast<UPrimitiveComponent>(Comp))
        {
            Prim->SetVisibility(false);
            Prim->SetHiddenInGame(true);
            Prim->SetCastShadow(false);
            Prim->SetComponentTickEnabled(false);

            Prim->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        }
    }
}
