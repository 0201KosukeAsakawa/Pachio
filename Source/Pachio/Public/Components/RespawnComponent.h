// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RespawnComponent.generated.h"


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PACHIO_API URespawnComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	URespawnComponent();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


	/** 現在のOwnerを初期位置にリセットする関数 */
	UFUNCTION(BlueprintCallable, Category = "Respawn")
	void RespawnOwnerAtInitialLocation();

private:

	/** 最初の位置を記録する */
	FVector InitialLocation;
};

