// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Objects/ControllableObjectBase.h"
#include "Interface/ActionControl/CharacterActionInterfaces.h"
#include "MoveControllableObject.generated.h"

/**
 * 
 */
UCLASS()
class PACHIO_API AMoveControllableObject :	public AControllableObjectBase, public IControllableMover
{
	GENERATED_BODY()
public:
	AMoveControllableObject();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)override;

	void Movement(const FInputActionValue& Value)override;
	virtual void Action(const FInputActionValue& Value)override;
	UFUNCTION()
	void OnFootBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);
	void OnFootEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
private:
	UPROPERTY()
	UMoveComponent* MoveComp;

	// �����̃g���K�[�R���|�[�l���g
	UPROPERTY(VisibleAnywhere)
	class UBoxComponent* FootTrigger;
	UPROPERTY()
	TArray<AActor*> AttachedActors;
};
