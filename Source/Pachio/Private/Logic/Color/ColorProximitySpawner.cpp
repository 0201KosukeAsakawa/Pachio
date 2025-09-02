// Fill out your copyright notice in the Description page of Project Settings.


#include "Logic/Color/ColorProximitySpawner.h"
#include "FunctionLibrary.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"

UColorProximitySpawner::UColorProximitySpawner()
{
    ToggleNiagaraActiveState(false);
    OffMesh();
}

bool  UColorProximitySpawner::OnColorMatched(const FLinearColor& FilterColor)
{
    ToggleNiagaraActiveState(true);
   
    OffMesh();
    return false;
}

bool UColorProximitySpawner::OnColorMismatched(const FLinearColor& FilterColor)
{
    ToggleNiagaraActiveState(false);
    OnMesh();
    return true;
}

void UColorProximitySpawner::OnMesh()
{
    if (bHide) return;

    AActor* Owner = GetOwner();
    if (!Owner) return;

    for (UActorComponent* Component : Owner->GetComponents())
    {
        if (Component->ComponentHasTag("HideTarget"))
        {
            // PrimitiveComponent なら視覚とコリジョンをオフ
            if (UPrimitiveComponent* Primitive = Cast<UPrimitiveComponent>(Component))
            {
                Primitive->SetVisibility(false, false);
                Primitive->SetCollisionEnabled(ECollisionEnabled::NoCollision);
            }

            // アクターコンポーネントであれば動作を停止
            if (Component->IsActive())
            {
                Component->Deactivate();
            }

            // もし Tick も止めたい場合
            Component->PrimaryComponentTick.SetTickFunctionEnable(false);
        }
    }

    ActiveEffect();

    bHide = true;
}

void UColorProximitySpawner::OffMesh()
{
    if (!bHide) return;

    AActor* Owner = GetOwner();
    if (!Owner) return;

    for (UActorComponent* Component : Owner->GetComponents())
    {
        if (Component->ComponentHasTag("HideTarget"))
        {
            // PrimitiveComponent なら視覚とコリジョンをオフ
            if (UPrimitiveComponent* Primitive = Cast<UPrimitiveComponent>(Component))
            {
                Primitive->SetVisibility(true, true);
                Primitive->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
            }

            // アクターコンポーネントであれば動作を停止
            if (Component->IsActive())
            {
                Component->Activate(true);
            }

            // もし Tick も止めたい場合
            Component->PrimaryComponentTick.SetTickFunctionEnable(true);
        }
    }

    bHide = false;
}
