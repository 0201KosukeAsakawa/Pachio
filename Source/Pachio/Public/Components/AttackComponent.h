#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AttackComponent.generated.h"

class UAttackStrategy;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PACHIO_API UAttackComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UAttackComponent();

    /** 新しい攻撃戦略を設定する */
    UFUNCTION(BlueprintCallable, Category = "Attack")
    void SetAttackStrategy(UAttackStrategy* NewStrategy);

    /** 対象アクターに攻撃を実行する */
    UFUNCTION(BlueprintCallable, Category = "Attack")
    void PerformAttack(AActor* Target);

    /** 攻撃力を取得する */
    float GetAttackPower() const;

protected:
    virtual void BeginPlay() override;

public:
    /** デフォルトで使用する攻撃戦略のクラス（インスタンスはBeginPlayで生成） */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
    TSubclassOf<UAttackStrategy> DefaultAttackStrategyClass;

    /** 現在使用中の攻撃戦略インスタンス */
    UPROPERTY(VisibleInstanceOnly, Category = "Attack")
    UAttackStrategy* CurrentStrategy;

protected:
    /** 基本攻撃力（各戦略で加算や補正が可能） */
    float BaseAttackPower = 1.0f;
};