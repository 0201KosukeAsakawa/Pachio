// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ItemEffectSource.h"
#include "Fireflower.generated.h"

/**
 * 
 */
UCLASS()
class PACHIO_API UFireflower : public UItemEffectSourceComponent
{
	GENERATED_BODY()


    // Called when the item is collected
    virtual void OnCollected(
        UPrimitiveComponent* OverlappedComponent,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex,
        bool bFromSweep,
        const FHitResult& SweepResult
    )override;

    // Initializes the effect source
    virtual void Init(AItemBase*)override;

    // Updates the effect source every tick or interval
    virtual void Update(float DeltaTime)override;

private:
    // エフェクト（例：プレイヤーが強化されるときのパーティクル）
    UPROPERTY(VisibleAnywhere)
    UParticleSystemComponent* PowerUpEffect;
};
