// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CameraHandlerComponent.generated.h"

class USpringArmComponent;
class UCameraComponent;

UENUM(BlueprintType)
enum class ECameraViewType : uint8
{
	GridView  UMETA(DisplayName = "Side View"),
	CharacterView   UMETA(DisplayName = "Top View")
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PACHIO_API UCameraHandlerComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	// Sets default values for this component's properties
	UCameraHandlerComponent();
public:
	void Init(TObjectPtr<USceneComponent>);
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void ApplyCameraSettings(FVector2D, float);
	void ApplyCameraSettings(FVector2D, float, ECameraViewType);
	void ChangeViewMode(ECameraViewType newMode) { CameraViewType = newMode; }
	bool IsParameterMatch(FVector2D, float);

	UCameraComponent* GetCamera() { return Camera; }
private:	
	void UpdateCameraPosition(float DeltaTime);
	void SetCameraRotation(ECameraViewType);
	void SetCameraLocation(ECameraViewType);
private:
	UPROPERTY(EditAnywhere, Category = "Grid")
	ECameraViewType CameraViewType = ECameraViewType::CharacterView;

	// グリッドサイズ（1部屋のサイズ）
	UPROPERTY(EditAnywhere, Category = "Grid")
	FVector2D GridSize = FVector2D(7000.f, 3000.f);
	UPROPERTY(EditAnywhere, Category = "Grid")
	float Zbaffa = 2000.f;

	FVector2D CurrentGridSize;
	float CurrentZbaffa;

	// 現在のグリッド座標（整数）
	FIntPoint CurrentGrid = FIntPoint::ZeroValue;

	// カメラ目標位置
	FVector TargetCameraLocation;

	// 補間速度（カメラ移動の滑らかさ）
	float InterpSpeed = 5.f;

	// ==== カメラ ====
	FVector CameraLocation;

	// カメラの回転／位置制御用スプリングアーム
	UPROPERTY(VisibleAnywhere, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> SpringArm;

	// プレイヤー視点用カメラ
	UPROPERTY(VisibleAnywhere, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> Camera;
};
