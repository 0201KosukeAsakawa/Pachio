// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Objects/ColorReactiveObject.h"
#include "MovingObject.generated.h"

UCLASS()
class PACHIO_API AMovingObject : public AColorReactiveObject
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMovingObject();

	virtual void Init()override;
	virtual void Tick(float)override;
private:
	virtual void ColorAction(const FLinearColor InColor = FLinearColor::White) override;

private:
	bool bIsMoving = false;
	UPROPERTY(EditAnywhere)
	FVector OffLocation;
};
