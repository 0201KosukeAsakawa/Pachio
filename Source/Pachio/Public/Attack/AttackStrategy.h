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

enum class EBreakLevel : uint8
{
    Unbreakable,  // 壊せない
    Functional,   // 作動できる
    Breakable     // 壊せる
};

// サウンドデータを格納する構造体
USTRUCT()
struct FAttackData : public FTableRowBase
{
    GENERATED_USTRUCT_BODY()

public:
    // サウンドウェーブとその対応するオーディオコンポーネントを保持
    EAttackType attackType;
    EBreakLevel breakLevel;
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
    virtual void ExecuteEffect(AActor*, AActor*,FAttackData,float FinalDamage = 0);
};