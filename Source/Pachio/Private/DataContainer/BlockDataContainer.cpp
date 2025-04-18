// Fill out your copyright notice in the Description page of Project Settings.


#include "DataContainer/BlockDataContainer.h"
#include "Components/BlockState.h"

UBlockState* UBlockDataContainer::CreateState(UObject* WorldContext, FString StateName) const
{
    // 直接 Find を使って、値が存在する場合にインスタンスを作成
    if (const TSubclassOf<UBlockState>* BlockStateClass = AttackStrategyMap.Find(StateName))
    {
        return NewObject<UBlockState>(WorldContext, *BlockStateClass);
    }
    return nullptr;
}