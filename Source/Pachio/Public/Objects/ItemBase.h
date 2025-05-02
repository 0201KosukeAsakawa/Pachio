// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemBase.generated.h"

class UItemEffectSourceComponent;
class UPhysicsCalculator;

UCLASS()
class PACHIO_API AItemBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItemBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	//
	void Init(FString objectID,const FString meshID = "None", const FString materialID = "None", const FVector direc = FVector(0,0,0));

	void AddForce(float force,FVector addDirection);
	 UFUNCTION()
    void OnCollected(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UPhysicsCalculator* GetPhysics()const { return physics; }

private:
	UPROPERTY()
	UItemEffectSourceComponent* ItemLogic;
	UPROPERTY(EditAnywhere)
	FVector direction;
	UPROPERTY()
	UPhysicsCalculator* physics;
	FString ObjectID;
	FString MaterialID;
	FString StaticMeshID;
};
