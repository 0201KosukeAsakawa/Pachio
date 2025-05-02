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

AItemBase* UItemDataContainer::GenerateItem(FString ItemID, FVector location, FVector direction, float force, FVector addDirection, FVector scale, FRotator rotation, const FString meshID, const FString materialID)
{
    // ItemClassMap内にItemIDが存在しない場合は早期リターン
    if (!ItemClassMap.Contains(ItemID))
    {
        return nullptr;
    }

    // ItemClass を取得
    UClass* itemClass = ItemClassMap[ItemID];
    if (!itemClass)
    {
        return nullptr;
    }

    // Worldを取得
    UWorld* World = GEngine->GetWorldFromContextObjectChecked(this);
    if (!World)
    {
        return nullptr;
    }

    // アイテムをスポーン
    AItemBase* NewItem = World->SpawnActor<AItemBase>(ItemClass, location, rotation);
    if (!NewItem)
    {
        return nullptr;
    }

    // スケールを設定
    NewItem->SetActorScale3D(scale);

    // アイテムの初期化
    NewItem->Init(ItemID, meshID, materialID, direction);

    // 力を加える
    NewItem->AddForce(force, addDirection);

    return NewItem;
}