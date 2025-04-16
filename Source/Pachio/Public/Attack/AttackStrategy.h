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

/**
 * 攻撃の戦略を定義する基底クラス
 */
UCLASS(Abstract, Blueprintable, EditInlineNew, DefaultToInstanced)
class PACHIO_API UAttackStrategy : public UObject
{
    GENERATED_BODY()

public:
    // 攻撃処理を実行
   virtual void ExecuteEffect(AActor* , AActor* );

protected:
    // 攻撃力
    float Damage;

    // オブジェクトを破壊できるか
    bool bIsDestructible;

    // 攻撃の種類
    EAttackType AttackType;
};