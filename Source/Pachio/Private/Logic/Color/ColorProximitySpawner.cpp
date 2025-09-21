// Fill out your copyright notice in the Description page of Project Settings.


#include "Logic/Color/ColorProximitySpawner.h"
#include "FunctionLibrary.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "NiagaraActor.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"

UColorProximitySpawner::UColorProximitySpawner()
{
    ToggleNiagaraActiveState(false);    
    bHide = false;
    OffMesh();
    
}

bool  UColorProximitySpawner::OnColorMatched(const FLinearColor& FilterColor)
{
    ToggleNiagaraActiveState(true);
    OnMesh();
    ActiveEffect(true);
    return false;
}

bool UColorProximitySpawner::OnColorMismatched(const FLinearColor& FilterColor)
{
    ToggleNiagaraActiveState(false);
    OffMesh();
    ActiveEffect(false);
    return true;
}

void UColorProximitySpawner::OffMesh()
{
    if (bHide) return;

    AActor* Owner = GetOwner();
    if (!Owner) return;

    for (UActorComponent* Component : Owner->GetComponents())
    {
        if (Component->ComponentHasTag("HideTarget"))
        {
            // PrimitiveComponent �Ȃ王�o�ƃR���W������I�t
            if (UPrimitiveComponent* Primitive = Cast<UPrimitiveComponent>(Component))
            {
                Primitive->SetVisibility(false, false);
                Primitive->SetCollisionEnabled(ECollisionEnabled::NoCollision);
            }

            // �A�N�^�[�R���|�[�l���g�ł���Γ�����~
            if (Component->IsActive())
            {
                Component->Deactivate();
            }

            // ��� Tick ��~�߂����ꍇ
            Component->PrimaryComponentTick.SetTickFunctionEnable(false);
        }
    }

    PlayAppearEffect();

    bHide = true;
}

void UColorProximitySpawner::OnMesh()
{
    if (!bHide) return;

    AActor* Owner = GetOwner();
    if (!Owner) return;

    for (UActorComponent* Component : Owner->GetComponents())
    {
        if (Component->ComponentHasTag("HideTarget"))
        {
            // PrimitiveComponent �Ȃ王�o�ƃR���W������I�t
            if (UPrimitiveComponent* Primitive = Cast<UPrimitiveComponent>(Component))
            {
                Primitive->SetVisibility(true, true);
                Primitive->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
            }

            // �A�N�^�[�R���|�[�l���g�ł���Γ�����~
            if (Component->IsActive())
            {
                Component->Activate(true);
            }

            // ��� Tick ��~�߂����ꍇ
            Component->PrimaryComponentTick.SetTickFunctionEnable(true);
        }
    }
    DeactivateAllEffects();
    bHide = false;
}
