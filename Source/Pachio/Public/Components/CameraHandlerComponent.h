// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CameraHandlerComponent.generated.h"

class USpringArmComponent;
class UCameraComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PACHIO_API UCameraHandlerComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	// Sets default values for this component's properties
	UCameraHandlerComponent();
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
public:
	void Init(TObjectPtr<USceneComponent>);
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
private:
	void UpdateCameraPosition(float DeltaTime);
	// 判定結果のゲッター
	//bool IsMovingLeft() const { return bIsMovingLeft; }
	//bool IsMovingRight() const { return bIsMovingRight; }

	//void SetMovingLeft(bool bLeft) { bIsMovingLeft = bLeft; }
	//void SetMovingRight(bool bRight) { bIsMovingRight = bRight; }
private:

	// ヘッダーに追加（必要に応じて）
	FVector InitialSpringArmOffset;
	float PreviousCameraY = 0.0f;
	//bool bIsMovingLeft = false;
	//bool bIsMovingRight = false;
	// ==== カメラ ====

	// カメラの回転／位置制御用スプリングアーム
	UPROPERTY(VisibleAnywhere, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> SpringArm;

	// プレイヤー視点用カメラ
	UPROPERTY(VisibleAnywhere, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> Camera;
};
