#include "DataContainer/AttackDataContainer.h"
#include "Components/AttackComponent.h"

UAttackStrategy* UAttackDataContainer::CreateStrategy(UObject* Outer, FName Type) const
{
    if (const TSubclassOf<UAttackStrategy>* FoundClass = AttackStrategyMap.Find(Type))
    {
        return NewObject<UAttackStrategy>(Outer, *FoundClass);
    }
    return nullptr;
}
 UAttackComponent* UAttackDataContainer::GenerateAttackComponent(AActor* Owner, FName Type)const
{
    if (!Owner)
    {
        UE_LOG(LogTemp, Warning, TEXT("GenerateAttackComponent failed: Owner is null"));
        return nullptr;
    }

    const TSubclassOf<UAttackComponent>* FoundClass = AttckComponentMap.Find(Type);
    if (!FoundClass || !(*FoundClass))
    {
        UE_LOG(LogTemp, Warning, TEXT("GenerateAttackComponent failed: No class found for type %s"), *Type.ToString());
        return nullptr;
    }

    // コンポーネントを所有アクターにアタッチして生成
    UAttackComponent* NewComp = NewObject<UAttackComponent>(Owner, *FoundClass);
    if (NewComp)
    {
        NewComp->RegisterComponent(); // アクターに登録（BeginPlayが呼ばれるように）
        return NewComp;
    }

    return nullptr;
}