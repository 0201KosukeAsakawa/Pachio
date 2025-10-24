// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DataContainer/ColorTargetTypes.h"
#include "GameFramework/Actor.h"
#include "PaintBall.generated.h"

class USphereComponent;

UCLASS()
class PACHIO_API APaintBall : public AActor
{
    GENERATED_BODY()
public:
    APaintBall();

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    UPROPERTY(EditAnywhere)
    FLinearColor PaintColor;

protected:
    UFUNCTION()
    void OnOverlap(AActor* OtherActor);

private:
    UPROPERTY(VisibleAnywhere)
    USphereComponent* CollisionSphere;
};

