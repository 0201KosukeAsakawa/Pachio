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

USTRUCT()
struct FAttackData : public FTableRowBase
{
    GENERATED_USTRUCT_BODY()

public:
    // 誰の発信の攻撃か
    EAttackType attackType;

    // ブロックに対する影響力
    EBreakLevel breakLevel;

    // 攻撃に必要な基本プロパティ
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack Strategy")
    float BaseDamage;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack Strategy")
    bool bIsProjectile;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack Strategy")
    bool bDestroyAfterHit;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
    UParticleSystem* AttackEffect;

    // コンストラクタで初期値を設定
    FAttackData()
        : attackType(EAttackType::Indiscriminate), breakLevel(EBreakLevel::Breakable), BaseDamage(10.f), bIsProjectile(false), bDestroyAfterHit(true), AttackEffect(nullptr)
    {
    }

};


UCLASS(Abstract, Blueprintable, EditInlineNew, DefaultToInstanced)
class PACHIO_API UAttackStrategy : public UObject
{
    GENERATED_BODY()

public:
    UAttackStrategy();

    // 攻撃の実行
    virtual void ExecuteEffect(AActor* Attacker, AActor* Target, FAttackData, float FinalDamage = 0);
};