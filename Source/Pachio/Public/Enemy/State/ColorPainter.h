// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/EnemyCharacter.h"
#include "DataContainer/EffectMatchResult.h"
#include "ColorPainter.generated.h"

/**
 * 
 */
UCLASS()
class PACHIO_API AColorPainter : public AEnemyCharacter
{
    GENERATED_BODY()
public:
    virtual void Tick(float DeltaTime) override;
    virtual void BeginPlay() override;
    virtual void Init() override;

private:
    UPROPERTY(EditAnywhere, Category = "Patrol")
    TArray<FVector> PatrolPoints;

    int32 CurrentPatrolIndex = 0;
    float AcceptanceRadius = 50.f; // 到達判定半径
};

