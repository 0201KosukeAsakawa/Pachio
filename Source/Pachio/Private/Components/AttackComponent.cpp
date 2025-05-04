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

    // ゲーム開始時にデフォルトの攻撃戦略をインスタンス化
    if (DefaultAttackStrategyClass)
    {
        CurrentStrategy = NewObject<UAttackStrategy>(this, DefaultAttackStrategyClass);
    }
}

bool UAttackComponent::SetAttackStrategy(FName NewStrategy)
{
    if (!pWorld)
        return false;

     UAttackStrategy* ua =   ALevelManager::GetInstance(GetWorld())->GetAttackDataContainer()->CreateStrategy(pWorld, NewStrategy);
    
     if (!ua)
         return false;

     CurrentStrategy = ua;

    return true;
}

float UAttackComponent::GetAttackPower() const
{
    // プレイヤーのレベルやバフなどを考慮する場合は、ここで補正処理を行う
    return BaseAttackPower;
}

void UAttackComponent::Init(UWorld* world)
{
    if (!world)
        return;

    pWorld = world;
}

const void UAttackComponent::PerformAttack(AActor* Target)
{
    // 現在の攻撃戦略が存在し、対象が有効な場合に攻撃を実行
    if (CurrentStrategy && Target)
    {
        // 戦略に応じた攻撃効果を実行（ノックバックやエフェクトも含めて処理）
        CurrentStrategy->ExecuteEffect(GetOwner(), Target, AttackType, BaseAttackPower);
    }
}