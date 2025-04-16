// Fill out your copyright notice in the Description page of Project Settings.


#include "DataContainer/AttackDataContainer.h"

UAttackStrategy* UAttackDataContainer::CreateStrategy(UObject* Outer, FString Type) const
{
    if (const TSubclassOf<UAttackStrategy>* FoundClass = AttackStrategyMap.Find(Type))
    {
        return NewObject<UAttackStrategy>(Outer, *FoundClass);
    }
    return nullptr;
}