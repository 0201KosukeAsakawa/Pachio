// Fill out your copyright notice in the Description page of Project Settings.


#include "Logic/Color/ColorTriggerStopComponent.h"
#include "GameFramework/Actor.h"

// Sets default values for this component's properties
UColorTriggerStopComponent::UColorTriggerStopComponent()
{

}

void UColorTriggerStopComponent::OnColorMatched()
{
    AActor* Owner = Cast<AActor>(GetOwner());
    if (!Owner)
        return;

    // --- 当たり判定（Collision）を無効化 ---
    TArray<UActorComponent*> CollisionComponents = GetOwner()->GetComponentsByClass(UPrimitiveComponent::StaticClass());
    for (UActorComponent* Comp : CollisionComponents)
    {
        UPrimitiveComponent* PrimComp = Cast<UPrimitiveComponent>(Comp);
        if (PrimComp)
        {
            // 衝突無効化
            PrimComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        }
    }

    // --- 見た目（メッシュ等）を非表示に ---
    TArray<UActorComponent*> MeshComponents = Owner->GetComponentsByClass(UMeshComponent::StaticClass());
    for (UActorComponent* Comp : MeshComponents)
    {
        UMeshComponent* MeshComp = Cast<UMeshComponent>(Comp);
        if (MeshComp)
        {
            MeshComp->SetVisibility(false, true);  // 子も含めて非表示
            MeshComp->SetHiddenInGame(true);
        }
    }
}
