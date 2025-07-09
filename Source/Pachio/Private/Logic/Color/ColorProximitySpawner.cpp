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

    // アクター表示 & Tick 再開
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
        }
    }
}

void UColorProximitySpawner::OffMesh()
{
    AActor* Owner = GetOwner();
    if (!Owner) return;

    // アクター非表示 & Tick 停止
    Owner->SetActorHiddenInGame(true);
    Owner->SetActorTickEnabled(false);

    // コンポーネントを調整
   for (UActorComponent* Comp : Owner->GetComponents())
{
    if (UPrimitiveComponent* Prim = Cast<UPrimitiveComponent>(Comp))
    {
        Prim->SetVisibility(false);
        Prim->SetHiddenInGame(true);
        Prim->SetCastShadow(false);
        Prim->SetComponentTickEnabled(false);

        Prim->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    }
}
}
