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

    // アクター非表示 & Tick 停止
    Owner->SetActorHiddenInGame(true);
    Owner->SetActorTickEnabled(false);

    // コンポーネントを調整
    for (UActorComponent* Comp : Owner->GetComponents())
    {
        if (UPrimitiveComponent* Prim = Cast<UPrimitiveComponent>(Comp))
        {
            // 見た目と影を消す
            Prim->SetVisibility(false);
            Prim->SetHiddenInGame(true);
            Prim->SetCastShadow(false);

            // Tick 停止（必要なければ）
            Prim->SetComponentTickEnabled(false);

            // 衝突は Overlap のみ許可（QueryOnly）
            Prim->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

            // 一旦すべて無視
            Prim->SetCollisionResponseToAllChannels(ECR_Ignore);

            // 例：Pawn にのみ Overlap 反応させたい場合（必要に応じて調整）
            Prim->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
        }
    }
}

void UColorTriggerStopComponent::OnColorMismatched(const FLinearColor& FilterColor)
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

            // 通常の衝突有効化（物理含む）
            Prim->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
            Prim->SetCollisionResponseToAllChannels(ECR_Block);  // もとの挙動に応じて変更
        }
    }
}
