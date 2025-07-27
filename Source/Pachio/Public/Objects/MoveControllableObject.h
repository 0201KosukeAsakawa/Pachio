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
class PACHIO_API AMoveControllableObject : public AControllableObjectBase, public IControllableMover
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
	UFUNCTION(BlueprintCallable)
	void OnFootEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
private:
	// 実移動処理
	void ExecuteMovement(const FVector& Direction);

	UFUNCTION()
	void OnBeatDetected();
	bool CanMoveToTarget(const FVector& Start, const FVector& End) const;
	FVector GetCollisionBoxExtent() const;
private:
	UPROPERTY()
	UMoveComponent* MoveComp;

	// �����̃g���K�[�R���|�[�l���g
	UPROPERTY(VisibleAnywhere)
	class UBoxComponent* FootTrigger;
	UPROPERTY()
	TArray<AActor*> AttachedActors;

	UPROPERTY(EditAnywhere)
	float MovementScale = 100.0f;
	// 入力値を保持
	FInputActionValue PendingInput;
	UPROPERTY(EditAnywhere)
	FVector MovementAxis;
	// 入力方向を保持する（正規化済）
	FVector CurrentInputDirection = FVector::ZeroVector;
	bool bHasInput = false;

	FVector StartLocation;
	FVector TargetLocation;
	float MoveDuration = 0.3f; // 移動にかける時間
	float ElapsedTime = 0.f;
	bool bIsMoving = false;
};
