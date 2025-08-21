// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Objects/Color/ColorReactiveObject.h"
#include "DataContainer/EffectMatchResult.h"
#include "MovingObject.generated.h"

class UBoxComponent;

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
	virtual void ColorAction(const FLinearColor InColor , FEffectMatchResult) override;

	UFUNCTION()
	void OnFootBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnFootEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

private:
	bool bIsMoving = false;
	UPROPERTY(EditAnywhere)
	FVector OffLocation;
	UPROPERTY(EditAnywhere)
	FVector OnLocation;
	FVector TargetLocation;
	UPROPERTY(EditAnywhere)
	TArray<AActor*> Child;

	// ��������p�̃g���K�[
	UPROPERTY(VisibleAnywhere)
	UBoxComponent* FootTrigger;

	// ����Ă���A�N�^�[�Ǘ��p���X�g
	TArray<AActor*> AttachedActors;

	FVector StartLocation;
	UPROPERTY(EditAnywhere)
	float MoveDuration = 1.0f; // 移動にかける秒数
	float ElapsedTime = 0.0f;
};
