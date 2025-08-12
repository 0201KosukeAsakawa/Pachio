// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Objects/Color/ColorReactiveObject.h"
#include "DataContainer/EffectMatchResult.h"
#include "RotationGroup.generated.h"

class AActor;

UCLASS()
class PACHIO_API ARotationGroup : public AColorReactiveObject
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARotationGroup();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
private:
	virtual void Init()override;
	virtual void Tick(float)override;
	virtual void ColorAction(const FLinearColor InColor,FEffectMatchResult) override;

	void UpdateBsRelativeToA(const FRotator& NewRotation);


private:
	UPROPERTY(EditAnywhere)
	TArray<FTargetData> TargetArray;

	UPROPERTY(EditAnywhere)
	FVector RotationAxis;

	UPROPERTY(EditAnywhere)
	bool bShouldRotate = false;
};
