// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Objects/Color/ColorReactiveObject.h"
#include "LadderActor.generated.h"

class UBoxComponent;

UCLASS()
class PACHIO_API ALadderActor : public AColorReactiveObject
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALadderActor();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	FVector GetTopWorldPosition() const;

	FVector GetBottomWorldPosition() const;

	FVector GetFixedPositionForActor( AActor* OtherActor) const;

	FRotator GetRotationForActorToFaceThis(AActor* OtherActor) const;


private:
	UPROPERTY(EditAnywhere)
	float Offset;

	UPROPERTY(EditAnywhere)
	UBoxComponent* LadderVolume;
};
