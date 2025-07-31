// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Objects/ControllableObjectBase.h"
#include "Interface/ColorFilterInterface.h"
#include "Interface/ColorReactionConfigInterface.h"
#include "Interface/ActionControl/CharacterActionInterfaces.h"
#include "MoveControllableObject.generated.h"

class UColorConfigurator;
class UCameraHandlerComponent;

UENUM(BlueprintType)
enum class EAxisType : uint8
{
	X UMETA(DisplayName = "X Axis"),
	Y UMETA(DisplayName = "Y Axis"),
	Z UMETA(DisplayName = "Z Axis")
};

UCLASS()
class PACHIO_API AMoveControllableObject :	public AControllableObjectBase, public IControllableMover
										 ,  public IColorReactiveInterface, public IColorReactionConfigInterface
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
	// インターフェース実装
	virtual void ColorAction(FLinearColor InColor) override;
	virtual void SetColor(FLinearColor)override;
	virtual void ResetColor()override;
	virtual void SetSelectMode(bool)override;
	virtual bool IsColorChange()const override;
	void ChangeLock(bool b) override;

private:
	// 実移動処理
	void ExecuteMovement(const FVector& Direction);
	void ApplyEffectFromColor(const FLinearColor& Color);
	UFUNCTION()
	void OnBeatDetected();
	bool CanMoveToTarget(const FVector& Start, const FVector& End) const;
	FVector GetCollisionBoxExtent() const;
private:
	UPROPERTY(EditAnywhere, Category = "Movement")
	UColorConfigurator* ColorConfigurator;
	UPROPERTY()
	UMoveComponent* MoveComp;
	UPROPERTY(EditAnywhere)
	UCameraHandlerComponent* CameraHandlerComponent;

	// �����̃g���K�[�R���|�[�l���g
	UPROPERTY()
	class UBoxComponent* FootTrigger;
	UPROPERTY()
	TArray<AActor*> AttachedActors;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float MovementScale = 100.0f;
	// 入力値を保持
	FInputActionValue PendingInput;
	UPROPERTY(EditAnywhere, Category = "Movement")
	FVector MovementAxis;
	// 入力方向を保持する（正規化済）
	FVector CurrentInputDirection = FVector::ZeroVector;
	FVector StartLocation;
	FVector TargetLocation;
	float MoveDuration = 0.3f; // 移動にかける時間
	float ElapsedTime = 0.f;
	bool bIsMoving = false;
	bool bHasInput = false;

	UPROPERTY(EditAnywhere, Category = "Movement")
	TArray<EAxisType> AllowedAxes;
};
