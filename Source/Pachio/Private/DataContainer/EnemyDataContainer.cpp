// Fill out your copyright notice in the Description page of Project Settings.


#include "DataContainer/EnemyDataContainer.h"
#include "Enemy/EnemyCharacter.h"

UEnemyStateComponent* UEnemyDataContainer::CreateState(UObject* WorldContext, FString StateName) const
{
    if (const TSubclassOf<UEnemyStateComponent>* BlockStateClass = BlockClassMap.Find(StateName))
    {
        return NewObject<UEnemyStateComponent>(WorldContext, *BlockStateClass);
    }
    return nullptr;
}

UMaterialInterface* UEnemyDataContainer::CreateMaterial(UObject* WorldContext, FString StateName)
{
    if (const TSoftObjectPtr<UMaterialInterface>* MaterialPtr = MaterialMap.Find(StateName))
    {
        return MaterialPtr->LoadSynchronous();
    }
    return nullptr;
}

bool UEnemyDataContainer::GenerateEnemy(FString stateID, FString dropItemID, FString materialID, FVector location, FVector scale, FRotator rotator)
{
    if (!EnemyCharacter) return false;

    UWorld* World = GEngine->GetWorldFromContextObjectChecked(this);
    if (!World) return false;

    AEnemyCharacter* NewEnemy = World->SpawnActor<AEnemyCharacter>(EnemyCharacter, location, rotator);
    if (!NewEnemy)
        return false;
    NewEnemy->SetActorScale3D(scale);
    // Init に this（コンテナ）を渡して、状態・マテリアルを内部で取得
    NewEnemy->Init(stateID, materialID);

    return true;
}
