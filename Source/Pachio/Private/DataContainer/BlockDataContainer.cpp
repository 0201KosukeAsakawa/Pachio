// Fill out your copyright notice in the Description page of Project Settings.


#include "DataContainer/BlockDataContainer.h"
#include "Components/BlockState.h"

UBlockState* UBlockDataContainer::CreateState(UObject* WorldContext, FString StateName) const
{
    // ���� Find ���g���āA�l�����݂���ꍇ�ɃC���X�^���X���쐬
    if (const TSubclassOf<UBlockState>* BlockStateClass = AttackStrategyMap.Find(StateName))
    {
        return NewObject<UBlockState>(WorldContext, *BlockStateClass);
    }
    return nullptr;
}