// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PhysicsCalculator.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PACHIO_API UPhysicsCalculator : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPhysicsCalculator();

protected:
	// ゲーム開始時に呼ばれる
	virtual void BeginPlay() override;

public:
	// 毎フレーム呼ばれる
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	void ResetForce();
	// オブジェクトに力を加える
	UFUNCTION(BlueprintCallable)
	void AddForce(FVector Direction, float Force, const bool bSweep = true, bool useLocalOffset = true);

	// 開始位置と終了位置から、オブジェクトが落下可能かを判断
	UFUNCTION(BlueprintCallable)
	bool OnGround() const;

	void SetGravityScale(const bool applyGravity = true, float scale = 9.8f , float Modifier = 1.0F);

	FVector GetBlockedAdjustedVector(const FVector& MoveVector);
	UFUNCTION(BlueprintCallable)
	const bool HasLanded();
	// 物理計算が有効かどうかを返す
	bool IsPhysicsEnabled() const { return bIsPhysicsEnabled; }
private:
	void UpdateGroundState();
	// オブジェクトに重力を加える
	void AddGravity();
	//設置面にあわせて傾ける
	FVector GetGroundNormal() const;

private:
	// 重力のスケールを設定（重力の強さ）
	float GravityScale = 9.8f;

	// 力の強さ（スケール）
	float ForceScale;

	UPROPERTY(EditAnywhere, Category = "Physics")
	float MaxFallingSpeed = 200.0f;

	// 力を加える方向
	FVector ForceDirection;

	// 前回のオブジェクトの位置（移動を比較するため）
	FVector PreviousPosition;	
	
	//
	FVector Velocity = FVector::ZeroVector;

	// 物理シミュレーションのタイマー（力の適用や時間ベースのロジックに使用）
	float Timer;

	float ForceModifier = 1;

	// 重力を加えるかどうかのフラグ
	UPROPERTY(EditAnywhere)
	bool bShouldApplyGravity = true;

	// スイープ衝突判定を使うかどうかのフラグ
	UPROPERTY(EditAnywhere)
	bool bIsSweep;

	// 物理計算が有効かどうかのフラグ
	UPROPERTY(EditAnywhere)
	bool bIsPhysicsEnabled;		

	bool bUseLocalOffset = true;

	bool bWasOnGround = false;

	bool bFalling = false;

	bool bHasJustLanded = false;

	bool bIgnoreGroundCheck = false; // 接地判定を一時的に無視するフラグ
};
