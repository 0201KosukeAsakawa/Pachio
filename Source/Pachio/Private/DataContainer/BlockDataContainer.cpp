// Fill out your copyright notice in the Description page of Project Settings.


#include "DataContainer/BlockDataContainer.h"
#include "Components/BlockState.h"
#include "Objects/BaseBlock.h"

UBlockState* UBlockDataContainer::CreateState(UObject* WorldContext, FString StateName) const
{
    // ���� Find ��g���āA�l�����݂���ꍇ�ɃC���X�^���X��쐬
    if (const TSubclassOf<UBlockState>* BlockStateClass = BlockClassMap.Find(StateName))
    {
        return NewObject<UBlockState>(WorldContext, *BlockStateClass);
    }
    return nullptr;
}

UMaterialInterface* UBlockDataContainer::CreateMaterial(UObject* WorldContext, FString StateName)
{
    if (const TSoftObjectPtr<UMaterialInterface>* MaterialPtr = MaterialMap.Find(StateName))
    {
        return MaterialPtr->LoadSynchronous();
    }
    return nullptr;
}

bool UBlockDataContainer::GenerateBlock(FString stateID, FString dropItemID, FString materialID, FVector location, FRotator rotator)
{
    if (!BlockClass) return false;

    UWorld* World = GEngine->GetWorldFromContextObjectChecked(this);
    if (!World) return false;

    ABaseBlock* NewBlock = World->SpawnActor<ABaseBlock>(BlockClass, location, rotator);
    if (!NewBlock) return false;

    // Init に this（コンテナ）を渡して、状態・マテリアルを内部で取得
    NewBlock->Init(stateID, dropItemID, materialID);

    return true;
}