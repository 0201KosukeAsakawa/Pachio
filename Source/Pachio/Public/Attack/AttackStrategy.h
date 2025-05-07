#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "AttackStrategy.generated.h"

UENUM(BlueprintType)
enum class EBreakLevel : uint8
{
    Unbreakable   UMETA(DisplayName = "Unbreakable"),
    Functional    UMETA(DisplayName = "Functional"),
    Breakable     UMETA(DisplayName = "Breakable")
};

UENUM(BlueprintType)
enum class EAttackType : uint8
{
    Player           UMETA(DisplayName = "Player"),
    Enemy            UMETA(DisplayName = "Enemy"),
    Indiscriminate   UMETA(DisplayName = "Indiscriminate")
};

USTRUCT()
struct FAttackData : public FTableRowBase
{
    GENERATED_USTRUCT_BODY()

public:
    // 誰の発信の攻撃か
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack Strategy")
    EAttackType attackType;

    // ブロックに対する影響力
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack Strategy")
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