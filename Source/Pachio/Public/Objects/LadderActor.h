// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Objects/ColorReactiveObject.h"
#include "LadderActor.generated.h"

class UBoxComponent;

UCLASS()
class PACHIO_API ALadderActor : public AColorReactiveObject
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALadderActor();

protected:
	virtual void Init()override;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	FVector GetTopWorldPosition() const;

	FVector GetBottomWorldPosition() const;

	UPROPERTY()
	UBoxComponent* LadderVolume;
};
