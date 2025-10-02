// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/CameraHandlerComponent.h"
#include "ConfigTriggerZone.generated.h"

UCLASS()
class PACHIO_API AConfigTriggerZone : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AConfigTriggerZone();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
    UFUNCTION()
    void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

private:
    UPROPERTY(VisibleAnywhere)
    class UBoxComponent* CollisionBox;
    UPROPERTY(EditAnywhere, Category = "Grid")
    ECameraViewType CameraViewType = ECameraViewType::GridView;
    UPROPERTY(EditAnywhere, Category = "Grid")
    FVector2D GridSize;
    UPROPERTY(EditAnywhere, Category = "Grid")
    float ZBaffer = 9000;
};
