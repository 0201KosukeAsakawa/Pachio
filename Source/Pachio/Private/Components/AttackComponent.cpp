#include "Components/AttackComponent.h"


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

void UAttackComponent::SetAttackStrategy(UAttackStrategy* NewStrategy)
{
    // 新しい戦略が有効であれば差し替える
    if (NewStrategy)
    {
        CurrentStrategy = NewStrategy;
    }
}

float UAttackComponent::GetAttackPower() const
{
    // プレイヤーのレベルやバフなどを考慮する場合は、ここで補正処理を行う
    return BaseAttackPower;
}

void UAttackComponent::PerformAttack(AActor* Target)
{
    // 現在の攻撃戦略が存在し、対象が有効な場合に攻撃を実行
    if (CurrentStrategy && Target)
    {
        // ダメージ計算（戦略が持つ基本ダメージ × プレイヤーの攻撃力）
        float FinalDamage = CurrentStrategy->GetBaseDamage() * GetAttackPower();

        // 戦略に応じた攻撃効果を実行（ノックバックやエフェクトも含めて処理）
        CurrentStrategy->ExecuteEffect(GetOwner(), Target, AttackType, FinalDamage);
    }
}