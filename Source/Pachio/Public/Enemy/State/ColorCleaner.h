// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/EnemyCharacter.h"
#include "ColorCleaner.generated.h"

/**
 * 
 */
UCLASS()
class PACHIO_API AColorCleaner : public AEnemyCharacter
{
    GENERATED_BODY()

public:
    AColorCleaner();

    virtual void Tick(float DeltaTime) override;
    virtual void BeginPlay() override;
    virtual void Init() override;

    // オーバーラップイベント
    void Overlap(AActor* OtherActor)override;

private:
    // ロジック関数
    AActor* FindTarget();
    void Wander(float DeltaTime);
    void MoveTowards(const FVector& Destination, float DeltaTime);
    bool IsInsideMoveRange(const FVector& Point) const;   
    FCollisionShape GetSimpleCollisionShape() const;
private:    
    UPROPERTY()
    AActor* TargetActor = nullptr;
    // 相対範囲（初期位置 ±）
    UPROPERTY(EditAnywhere, Category = "Search")
    FVector SearchAreaMin = FVector(-500.f, -500.f, 0.f);

    UPROPERTY(EditAnywhere, Category = "Search")
    FVector SearchAreaMax = FVector(500.f, 500.f, 200.f);

    // 状態
    FVector StartLocation;
    FVector WanderDirection = FVector::ZeroVector;
    float WanderCooldown = 0.f;
    bool bIsIdle = true;


};