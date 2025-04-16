#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "AttackStrategy.generated.h"

enum class EAttackType : uint8
{
    Player,
    Enemy,
    Indiscriminate,
};


UCLASS(Abstract, Blueprintable, EditInlineNew, DefaultToInstanced)
class PACHIO_API UAttackStrategy : public UObject
{
    GENERATED_BODY()

public:
    // ダメージ計算
    virtual float GetBaseDamage() const { return 0.f; }
    // 攻撃タイプ
    virtual EAttackType GetAttackType() const { return EAttackType::Indiscriminate; }
    // 攻撃処理を実行
    virtual void ExecuteEffect(AActor*, AActor*, float FinalDamage = 0);
};