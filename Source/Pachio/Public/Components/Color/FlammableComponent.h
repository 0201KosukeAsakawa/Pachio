// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Objects/Color/ColorReactiveObject.h"
#include "FlammableComponent.generated.h"

class UBoxComponent;
class UNiagaraSystem;
class UNiagaraComponent;

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
    UPROPERTY(EditAnywhere, Category = "Flame Effects")
    UNiagaraSystem* FlameSystem;

    // ���ۂɍĐ�����R���|�[�l���g
    UPROPERTY()
    UNiagaraComponent* FlameEffect;

    UPROPERTY(EditAnywhere)
    UBoxComponent* HitBox;
}
;
