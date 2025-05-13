#include "Components/AttackComponent.h"
#include "Attack/AttackStrategy.h"
#include "Manager/LevelManager.h"
#include "DataContainer/AttackDataContainer.h"

UAttackComponent::UAttackComponent()
{
    // このコンポーネントはTickを使わない（毎フレームの更新は不要）
    PrimaryComponentTick.bCanEverTick = false;
}

void UAttackComponent::BeginPlay()
{
    Super::BeginPlay();

}


float UAttackComponent::GetAttackPower() const
{
    // プレイヤーのレベルやバフなどを考慮する場合は、ここで補正処理を行う
    return AttackData.BaseDamage;
}

bool UAttackComponent::Init(UWorld* world , FName NewStrategy)
{
    if (!world)
        return false;

    UAttackStrategy* ua = ALevelManager::GetInstance(GetWorld())->GetAttackDataContainer()->CreateStrategy(world, NewStrategy);

    if (!ua)
        return false;

    CurrentStrategy = ua;

    return true;
}

const bool UAttackComponent::PerformAttack(AActor* Target)
{
    // 現在の攻撃戦略が存在し、対象が有効な場合に攻撃を実行
    if (CurrentStrategy && Target)
    {
        // 戦略に応じた攻撃効果を実行（ノックバックやエフェクトも含めて処理）
        return CurrentStrategy->ExecuteEffect(GetOwner(), Target, AttackData, AttackData.BaseDamage);
    }
    return false;
}