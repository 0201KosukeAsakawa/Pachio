#include "DataContainer/AttackDataContainer.h"

UAttackStrategy* UAttackDataContainer::CreateStrategy(UObject* Outer, FName Type) const
{
    if (const TSubclassOf<UAttackStrategy>* FoundClass = AttackStrategyMap.Find(Type))
    {
        return NewObject<UAttackStrategy>(Outer, *FoundClass);
    }
    return nullptr;
}