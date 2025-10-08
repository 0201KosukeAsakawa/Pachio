// Fill out your copyright notice in the Description page of Project Settings.


#include "DataContainer/EnemyDataContainer.h"
#include "Enemy/EnemyCharacter.h"

UEnemyStateComponent* UEnemyDataContainer::CreateState(UObject* WorldContext, EEnemyCategory StateName) const
{
    return nullptr;
}

UMaterialInterface* UEnemyDataContainer::CreateMaterial(UObject* WorldContext, EEnemyCategory StateName, FString Type)
{
    // StateName �ɑΉ����� FMaterialData �� MaterialMap ����擾
    if (const FMaterialData* MaterialData = MaterialMap.Find(StateName))
    {
        // Type �ɑΉ�����}�e���A����擾
        if (const TSoftObjectPtr<UMaterialInterface>* MaterialPtr = MaterialData->material.Find(Type))
        {
            // �}�e���A���𓯊��I�Ƀ��[�h���ĕԂ�
            return MaterialPtr->LoadSynchronous();
        }
    }

    // ������Ȃ������ꍇ�� nullptr ��Ԃ�
    return nullptr;
}


bool UEnemyDataContainer::GenerateEnemy(EEnemyCategory stateID, FString dropItemID, EEnemyCategory materialID, FVector location, FVector scale, FRotator rotator)
{
    if (!EnemyCharacter) return false;

    UWorld* World = GEngine->GetWorldFromContextObjectChecked(this);
    if (!World) return false;

    AEnemyCharacter* NewEnemy = World->SpawnActor<AEnemyCharacter>(EnemyCharacter, location, rotator);
    if (!NewEnemy)
        return false;
    NewEnemy->SetActorScale3D(scale);
    // Init �� this�i�R���e�i�j��n���āA��ԁE�}�e���A�������Ŏ擾
    NewEnemy->Init();

    return true;
}
