// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ItemEffectSource.generated.h"

class UPhysicsCalculator;
class AItemBase;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PACHIO_API UItemEffectSourceComponent : public UActorComponent
{
    GENERATED_BODY()

public:

    // Called when the item is collected
    virtual void OnCollected(
        UPrimitiveComponent* OverlappedComponent,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex,
        bool bFromSweep,
        const FHitResult& SweepResult
    );

    // Sets the direction for some effect
    virtual void SetDirection(FVector Direction);

    // Initializes the effect source
    virtual void Init(AItemBase*);

    // Updates the effect source every tick or interval
    virtual void Update(float DeltaTime);
protected:
    UPROPERTY()
    AItemBase* mOwner;
};