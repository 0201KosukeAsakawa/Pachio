// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/ActionControl/CharacterActionInterfaces.h"
#include "ControllableObjectBase.generated.h"
class UInputComponent;
class UMoveComponent;
UCLASS()
class PACHIO_API AControllableObjectBase :	public APawn , public IControllableMover,
											public IControllableAbility
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AControllableObjectBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)override;

	void Movement(const FInputActionValue& Value)override;
	void Action(const FInputActionValue& Value)override;
	UFUNCTION()
	void OnFootBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);
	void OnFootEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
private:
	UPROPERTY()
	UMoveComponent* MoveComp;

	// 足元のトリガーコンポーネント
	UPROPERTY(VisibleAnywhere)
	class UBoxComponent* FootTrigger;
	UPROPERTY()
	TArray<AActor*> AttachedActors;
};
