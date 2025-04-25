// Fill out your copyright notice in the Description page of Project Settings.


#include "Manager/ObjectManager.h"
#include "Objects/BaseBlock.h"

void UObjectManager::GenerateObject(FString ActorKey, FVector location, FRotator rotation)
{
    // TMap から指定されたキーに基づいてアクターのクラスを取得
    TSubclassOf<AActor>* ActorClass = floorClass.Find(ActorKey);

    if (ActorClass != nullptr)
    {
        // アクターのクラスが見つかった場合、アクターを生成
        AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(*ActorClass, location, rotation);

        if (SpawnedActor)
        {
            // 成功した場合にアクターの初期化などを行うことができます
            UE_LOG(LogTemp, Log, TEXT("Actor spawned successfully!"));
        }
        else
        {
            // 生成失敗時のエラーハンドリング
            UE_LOG(LogTemp, Error, TEXT("Failed to spawn actor!"));
        }
    }
    else
    {
        // クラスが見つからない場合のエラーハンドリング
        UE_LOG(LogTemp, Error, TEXT("Actor class not found for key: %s"), *ActorKey);
    }
}

void UObjectManager::GenerateBlock(FString stateID, FString dorpItemID, FString materialID, FVector location, FRotator rotation)
{
    //if (BaseBlock == nullptr)
    //{
    //    UE_LOG(LogTemp, Error, TEXT("BaseBlock class is not set!"));
    //    return;
    //}

    //// TSubclassOf<ABaseBlock> から UClass* を取得
    //UClass* ActorClass = BaseBlock.Get();

    //// アクターのクラスが見つかった場合、アクターを生成
    //ABaseBlock* SpawnedActor = GetWorld()->SpawnActor<ABaseBlock>(ActorClass, location, rotation);

    //if (SpawnedActor)
    //{
    //    // 成功した場合にアクターの初期化などを行うことができます
    //    UE_LOG(LogTemp, Log, TEXT("Actor spawned successfully!"));
    //}
    //else
    //{
    //    // 生成失敗時のエラーハンドリング
    //    UE_LOG(LogTemp, Error, TEXT("Failed to spawn actor!"));
    //}

    //// アクターの初期化処理
    //SpawnedActor->Init(stateID, dorpItemID, materialID);
}
