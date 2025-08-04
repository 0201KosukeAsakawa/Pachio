// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Objects/ColorReactiveObject.h"
#include "FlammableComponent.generated.h"

class UBoxComponent;
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PACHIO_API UFlammableComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UFlammableComponent();
    void BeginPlay();
    
    UFUNCTION(BlueprintCallable)
    void Ignite();

    UFUNCTION(BlueprintCallable)
    void Extinguish();

private:
    UFUNCTION()
    void OnOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
        bool bFromSweep, const FHitResult& SweepResult);
    UPROPERTY(EditAnywhere)
    bool bIsIgnited = false;

    UPROPERTY(EditAnywhere)
    bool bKillsOnTouch = true;

protected:
    UPROPERTY(EditAnywhere)
    TSubclassOf<AActor> FireEffectActor; // Niagara or particle

    UPROPERTY()
    AActor* SpawnedFire = nullptr;


    UPROPERTY(EditAnywhere)
    UBoxComponent* HitBox;
}
;
