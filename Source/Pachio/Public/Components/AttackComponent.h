#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Attack/AttackStrategy.h"
#include "AttackComponent.generated.h"


UCLASS(Blueprintable)
class PACHIO_API UAttackComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UAttackComponent();

    /** 対象アクターに攻撃を実行する */
    UFUNCTION(BlueprintCallable, Category = "Attack")
    const bool PerformAttack(AActor* Target);

    /** 攻撃力を取得する */
    float GetAttackPower() const;
    /** 新しい攻撃戦略を設定する */
    UFUNCTION(BlueprintCallable, Category = "Attack")
    bool Init(UWorld* world, FName NewStrategy);

protected:
    virtual void BeginPlay() override;

public:
    /** 現在使用中の攻撃戦略インスタンス */
    UPROPERTY(VisibleInstanceOnly, Category = "Attack")
    UAttackStrategy* CurrentStrategy;

protected:
    UPROPERTY(EditAnywhere)
    FAttackData AttackData;
    UPROPERTY()
    UAttackStrategy* CurrentState;
};