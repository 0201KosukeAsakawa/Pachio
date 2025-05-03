// Fill out your copyright notice in the Description page of Project Settings.

#include "DataContainer/ItemDataContainer.h"
#include "Interface/ItemEffectSource.h"
#include "Objects/ItemBase.h"

// CreateState関数: 指定されたStateNameに基づいて、アイテムのエフェクトソースコンポーネントを生成
UItemEffectSourceComponent* UItemDataContainer::CreateState(UObject* WorldContext, FString StateName) const
{
    // StateNameに対応するアイテムのクラスがマップ内にあるか確認
    if (const TSubclassOf<UItemEffectSourceComponent>* BlockStateClass = ItemClassMap.Find(StateName))
    {
        // 新しいエフェクトソースコンポーネントを生成
        return NewObject<UItemEffectSourceComponent>(WorldContext, *BlockStateClass);
    }
    // クラスが見つからなければnullptrを返す
    return nullptr;
}

// CreateMaterial関数: StateNameに基づいてアイテムのマテリアルを生成
UMaterialInterface* UItemDataContainer::CreateMaterial(UObject* WorldContext, FString StateName)
{
    // StateNameに対応するマテリアルがマップ内にあるか確認
    if (const TSoftObjectPtr<UMaterialInterface>* MaterialPtr = MaterialMap.Find(StateName))
    {
        // マテリアルを同期的にロードして返す
        return MaterialPtr->LoadSynchronous();
    }
    // マテリアルが見つからなければnullptrを返す
    return nullptr;
}

// GenerateItem関数: アイテムを生成して初期化する
AItemBase* UItemDataContainer::GenerateItem(FString ItemID, FVector location, FVector direction, float force, FVector addDirection, FVector scale, FRotator rotation, const FString meshID, const FString materialID)
{
    // ItemIDに対応するアイテムがItemClassMap内に存在しない場合、nullptrを返す
    if (!ItemClassMap.Contains(ItemID))
    {
        return nullptr;
    }

    // アイテムのクラスを取得
    UClass* itemClass = ItemClassMap[ItemID];
    if (!itemClass)
    {
        return nullptr;
    }

    // 現在のWorldを取得
    UWorld* World = GEngine->GetWorldFromContextObjectChecked(this);
    if (!World)
    {
        return nullptr;
    }

    // アイテムを指定した位置と回転でスポーン
    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

     AItemBase* NewItem = World->SpawnActor<AItemBase>(ItemClass, location, rotation, SpawnParams);
    if (!NewItem)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to spawn item. itemClass: %s, location: %s, rotation: %s"), *itemClass->GetName(), *location.ToString(), *rotation.ToString());
        return nullptr;
    }

    // アイテムのスケールを設定
    NewItem->SetActorScale3D(scale);

    // アイテムを初期化（ID, メッシュID, マテリアルID、方向を渡す）
    NewItem->Init(ItemID, meshID, materialID, direction);

    // アイテムに力を加える
    NewItem->AddForce(force, addDirection);

    // 生成したアイテムを返す
    return NewItem;
}