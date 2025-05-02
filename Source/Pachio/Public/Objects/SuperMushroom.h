// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interface/ItemEffectSource.h"
#include "SuperMushroom.generated.h"


class UMoveComponent;
class UPhysicsCalculator;
class AItemBase;

UCLASS()
class PACHIO_API USuperMushroomComponent : public UItemEffectSourceComponent
{
    GENERATED_BODY()

public:
    // コンストラクタ
    USuperMushroomComponent();
    // 初期化処理
     void Init(AItemBase*)override;
    // Tick関数
    void Update(float DeltaTime)override ;
    // オーバーラップイベントにバインドされる関数
    UFUNCTION()
    void OnCollected(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)override;
    virtual void SetDirection(FVector)override;
private:
    // エフェクト（例：プレイヤーが強化されるときのパーティクル）
    UPROPERTY(VisibleAnywhere)
    UParticleSystemComponent* PowerUpEffect;
    UPROPERTY()
    UMoveComponent* moveComp;
};