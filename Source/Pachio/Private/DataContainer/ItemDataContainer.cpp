// ItemDataContainer.cpp

#include "DataContainer/ItemDataContainer.h"
#include "Components/ItemEffectSource.h"
#include "Objects/ItemBase.h"

// 指定された StateName に対応する UItemEffectSourceComponent の派生クラスを生成して返す
// 登録されていない場合は nullptr を返す
UItemEffectSourceComponent* UItemDataContainer::CreateState(UObject* WorldContext, FString StateName) const
{
    if (const TSubclassOf<UItemEffectSourceComponent>* BlockStateClass = ItemClassMap.Find(StateName))
    {
        return NewObject<UItemEffectSourceComponent>(WorldContext, *BlockStateClass);
    }
    return nullptr;
}

// 指定された StateName に対応するマテリアルアセットを同期的に読み込み、UMaterialInterface として返す
// 該当のマテリアルが存在しない、または読み込みに失敗した場合は nullptr を返す
// ※同期読み込みはゲーム中に使用するとパフォーマンスに影響を与える可能性があるため注意
UMaterialInterface* UItemDataContainer::CreateMaterial(UObject* WorldContext, FString StateName)
{
    if (const TSoftObjectPtr<UMaterialInterface>* MaterialPtr = MaterialMap.Find(StateName))
    {
        return MaterialPtr->LoadSynchronous();
    }
    return nullptr;
}

// 指定された ItemID に対応するアイテムを生成し、初期化して返す
//
// 初期化内容:
// - スケール、位置、回転の設定
// - ID、メッシュID、マテリアルID によるビジュアル初期化
// - 初期移動方向と力の加算
//
// @param ItemID        : アイテムの識別子（ItemClassMap に登録されているキー）
// @param location      : ワールド内での生成位置
// @param direction     : アイテムの初期進行方向
// @param force         : 加える物理的な力の大きさ
// @param addDirection  : 力を加える方向ベクトル
// @param scale         : スケーリング（拡大縮小）係数
// @param rotation      : 初期回転
// @param meshID        : 表示用のメッシュ識別子
// @param materialID    : 表示用のマテリアル識別子
//
// @return              : 成功時は AItemBase のインスタンス、失敗時は nullptr
AItemBase* UItemDataContainer::GenerateItem(FString ItemID, FVector location, FVector direction, float force, FVector addDirection, FVector scale, FRotator rotation, const FString meshID, const FString materialID)
{
    // ItemID に対応するクラスが登録されていなければ nullptr を返す
    if (!ItemClassMap.Contains(ItemID))
    {
        return nullptr;
    }

    // アイテムクラス取得
    UClass* itemClass = ItemClassMap[ItemID];
    if (!itemClass)
    {
        return nullptr;
    }

    // World オブジェクトの取得
    UWorld* World = GEngine->GetWorldFromContextObjectChecked(this);
    if (!World)
    {
        return nullptr;
    }

    // アイテムのスポーンパラメータ設定
    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

    // アイテムの生成
    AItemBase* NewItem = World->SpawnActor<AItemBase>(itemClass, location, rotation, SpawnParams);
    if (!NewItem)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to spawn item. itemClass: %s, location: %s, rotation: %s"),
            *itemClass->GetName(), *location.ToString(), *rotation.ToString());
        return nullptr;
    }

    // スケール設定
    NewItem->SetActorScale3D(scale);

    // 初期化（ID、メッシュ、マテリアル、進行方向）
    NewItem->Init(ItemID, meshID, materialID, direction);

    // 力を加える
    NewItem->AddForce(force, addDirection);

    return NewItem;
}
