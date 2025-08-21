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

    // �I�[�o�[���b�v�C�x���g
    void Overlap(AActor* OtherActor)override;

private:
    UPROPERTY(EditAnywhere, Category = "Patrol")
    TArray<FVector> PatrolPoints;

    int32 CurrentPatrolIndex = 0;
    float AcceptanceRadius = 50.f; // �ړI�n�ɋ߂Â����Ƃ݂Ȃ�����
    UPROPERTY(EditAnywhere)
    FLinearColor PainterColor;
    UPROPERTY(EditAnywhere)
    FEffectMatchResult Match;
};
