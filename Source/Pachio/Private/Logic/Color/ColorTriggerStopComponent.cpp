// Fill out your copyright notice in the Description page of Project Settings.


#include "Logic/Color/ColorTriggerStopComponent.h"
#include "FunctionLibrary.h"
#include "Manager/LevelManager.h"
#include "Manager/ColorManager.h"
#include "Components/BoxComponent.h"



// Sets default values for this component's properties
UColorTriggerStopComponent::UColorTriggerStopComponent()
{

}

bool UColorTriggerStopComponent::OnColorMatched(const FLinearColor& FilterColor)
{
    if (bHide) return false;

    AActor* Owner = GetOwner();
    if (!Owner) return false;

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

    return bHide;
}


bool UColorTriggerStopComponent::OnColorMismatched(const FLinearColor& FilterColor)
{
    if (!bHide) return false;

    AActor* Owner = GetOwner();
    if (!Owner) return false;
    DeactivateAllEffects();
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

    
   return bHide = false;
}