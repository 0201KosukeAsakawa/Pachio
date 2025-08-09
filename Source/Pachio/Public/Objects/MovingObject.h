// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Objects/Color/ColorReactiveObject.h"
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
	virtual void ColorAction(const FLinearColor InColor = FLinearColor::White) override;

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

	// 足元判定用のトリガー
	UPROPERTY(VisibleAnywhere)
	UBoxComponent* FootTrigger;

	// 乗っているアクター管理用リスト
	TArray<AActor*> AttachedActors;
};
