// Fill out your copyright notice in the Description page of Project Settings.


#include "Logic/Color/ColorTriggerStopComponent.h"
#include "FunctionLibrary.h"
#include "Components/BoxComponent.h"



// Sets default values for this component's properties
UColorTriggerStopComponent::UColorTriggerStopComponent()
{

}

bool UColorTriggerStopComponent::OnColorMatched(const FLinearColor& FilterColor)
{
    AActor* Owner = GetOwner();
    if (!Owner) return false;
    ToggleNiagaraActiveState(false);
    // �A�N�^�[��\��
    Owner->SetActorHiddenInGame(true);
    // Tick��~
    Owner->SetActorTickEnabled(false);
    // �����蔻��I�t
    Owner->SetActorEnableCollision(false);

    // �O�̂��߁A�S�R���|�[�l���g��Tick��I�t����\�����R���W�����Ȃ��ɂ���
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

    return true;
}

bool UColorTriggerStopComponent::OnColorMismatched(const FLinearColor& FilterColor)
{
    AActor* Owner = GetOwner();
    if (!Owner) return false;
    ToggleNiagaraActiveState(true);
    // �A�N�^�[�ĕ\��
    Owner->SetActorHiddenInGame(false);
    // Tick�ėL����
    Owner->SetActorTickEnabled(true);
    // �����蔻��ėL����
    Owner->SetActorEnableCollision(true);

    // �S�R���|�[�l���g����ɖ߂�
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

    return false;
}
