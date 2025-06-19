// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Objects/ColorReactiveObject.h"
#include "ColorReactiveBeltConveyor.generated.h"

class UBoxComponent;
class UPhysicsCalculator;
/**
 * 
 */
UCLASS()
class PACHIO_API AColorReactiveBeltConveyor : public AColorReactiveObject
{
	GENERATED_BODY()
public:
	AColorReactiveBeltConveyor();
	virtual void Init()override;
	virtual void Tick(float)override;
private:
	virtual void ColorAction(const FLinearColor InColor = FLinearColor::White) override;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
private:
	// Box Component ‚ð’è‹`
	UPROPERTY(VisibleAnywhere)
	UBoxComponent* BoxComponent;
	UPROPERTY(EditAnywhere)
	FVector direction;
	UPROPERTY(EditAnywhere)
	float power = 0;
	FVector CurrentDirection;

	UPROPERTY()
	TArray<UPhysicsCalculator*> hitObject;
};
