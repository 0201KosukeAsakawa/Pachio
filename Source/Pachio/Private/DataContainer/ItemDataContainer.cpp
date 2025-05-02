// Fill out your copyright notice in the Description page of Project Settings.


#include "DataContainer/ItemDataContainer.h"
#include "Interface/ItemEffectSource.h"
#include "Objects/ItemBase.h"

UItemEffectSourceComponent* UItemDataContainer::CreateState(UObject* WorldContext, FString StateName) const
{
    if (const TSubclassOf<UItemEffectSourceComponent>* BlockStateClass = ItemClassMap.Find(StateName))
    {
        return NewObject<UItemEffectSourceComponent>(WorldContext, *BlockStateClass);
    }
    return nullptr;
}

UMaterialInterface* UItemDataContainer::CreateMaterial(UObject* WorldContext, FString StateName)
{
    if (const TSoftObjectPtr<UMaterialInterface>* MaterialPtr = MaterialMap.Find(StateName))
    {
        return MaterialPtr->LoadSynchronous();
    }
    return nullptr;
}

AItemBase* UItemDataContainer::GenerateItem(FString ItemID, FVector location, FVector direction, float force , FVector addDirection,FVector scale, FRotator rotation , const FString meshID , const FString materialID)
{
    if (!ItemClass)
        return nullptr;

    UWorld* World = GEngine->GetWorldFromContextObjectChecked(this);
    if (!World)
        return nullptr;

    AItemBase* NewItem = World->SpawnActor<AItemBase>(ItemClass, location, rotation);
    if (!NewItem)
        return nullptr;
    NewItem->SetActorScale3D(scale);
    // Init に this（コンテナ）を渡して、状態・マテリアルを内部で取得
    NewItem->Init(ItemID,meshID, materialID, direction);
    NewItem->AddForce(force , addDirection);

    return NewItem;
}