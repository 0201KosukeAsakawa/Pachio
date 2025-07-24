// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Objects/ControllableObjectBase.h"
#include "RotateControllableObject.generated.h"

USTRUCT(BlueprintType)
struct FTargetData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "TargetData")
	AActor* targetActor = nullptr;
	UPROPERTY(EditAnywhere, Category = "TargetData")
	float rotateSpeed = 0;
};

UCLASS()
class PACHIO_API ARotateControllableObject : public AControllableObjectBase, public IControllableMover
{
	GENERATED_BODY()
public:
	ARotateControllableObject();
	void BeginPlay();
	void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)override;

	void Movement(const FInputActionValue& Value)override;
	virtual void Action(const FInputActionValue& Value)override;
	void UpdateBsRelativeToA(const FRotator& DeltaRotation);

private:
	UPROPERTY(EditAnywhere)
	TArray<FTargetData> TargetArray;
	UPROPERTY(EditAnywhere)
	bool bShouldRotate = false;
	UPROPERTY(EditAnywhere)
	FRotator direction;
};
