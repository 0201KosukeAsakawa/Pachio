// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Objects/Color/ColorReactiveObject.h"
#include "DataContainer/EffectMatchResult.h"
#include "MoveControllableObject.generated.h"


class UBoxComponent;
class UColorConfigurator;
class UCameraHandlerComponent;
UCLASS()
class PACHIO_API AMoveControllableObject : public AColorReactiveObject
{
	GENERATED_BODY()

public:
	AMoveControllableObject();

protected:
	virtual void Init() override;

public:
	virtual void Tick(float DeltaTime) override;

private:// オーバーラップイベント
	UFUNCTION()
	void OnFootBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintCallable)
	void OnFootEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void OnBeatDetected();

	virtual void ColorAction(FLinearColor, FEffectMatchResult)override;
private:

	UPROPERTY()
	UBoxComponent* FootTrigger;

	UPROPERTY()
	TArray<AActor*> AttachedActors;

	// パトロール関連
	UPROPERTY(EditAnywhere, Category = "Patrol")
	TArray<FVector> PatrolPoints;



	int32 CurrentPatrolIndex = 0;

	// 移動関連
	UPROPERTY(EditAnywhere, Category = "Movement")
	float MoveStepSize = 0.0f;

		UPROPERTY(EditAnywhere, Category = "Patrol")
	float AcceptanceRadius = 50.f;
	float MoveDuration = 0.2f; // 移動にかける時間（秒）
	float MoveElapsedTime = 0.0f;
	FVector MoveStartLocation;
	FVector MoveTargetLocation;

	float PlayCount = 0;
	UPROPERTY(EditAnywhere)
	float BeatCount = 0;

	UPROPERTY(EditAnywhere)
	bool bUseStepMove = true;
	bool bIsMoving = false;
	bool bCanMove = true;
};