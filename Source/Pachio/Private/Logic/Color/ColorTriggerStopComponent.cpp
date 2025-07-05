// Fill out your copyright notice in the Description page of Project Settings.


#include "Logic/Color/ColorTriggerStopComponent.h"
#include "FunctionLibrary.h"
#include "Components/BoxComponent.h"



// Sets default values for this component's properties
UColorTriggerStopComponent::UColorTriggerStopComponent()
{

}

void UColorTriggerStopComponent::OnColorMatched(const FLinearColor& FilterColor)
{
    AActor* Owner = GetOwner();
    if (!Owner) return;

    // アクター非表示
    Owner->SetActorHiddenInGame(true);
    // Tick停止
    Owner->SetActorTickEnabled(false);
    // 当たり判定オフ
    Owner->SetActorEnableCollision(false);

    // 念のため、全コンポーネントのTickもオフ＆非表示＆コリジョンなしにする
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

void UColorTriggerStopComponent::OnColorMismatched(const FLinearColor& FilterColor)
{
    AActor* Owner = GetOwner();
    if (!Owner) return;

    // アクター再表示
    Owner->SetActorHiddenInGame(false);
    // Tick再有効化
    Owner->SetActorTickEnabled(true);
    // 当たり判定再有効化
    Owner->SetActorEnableCollision(true);

    // 全コンポーネントも元に戻す
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
