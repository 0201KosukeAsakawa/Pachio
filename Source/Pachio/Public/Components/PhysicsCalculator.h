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

	// オブジェクトに力を加える
	void AddForce(FVector Direction, float Force, const bool bSweep = true);

	void ResetForce();

	// オブジェクトに重力を加える
	void AddGravity();

	// 開始位置と終了位置から、オブジェクトが落下可能かを判断
	bool CanFall(FVector Start, FVector End) const;

	// 物理計算が有効かどうかを返す
	bool IsPhysicsEnabled() const { return bIsPhysicsEnabled; }

private:
	// 重力のスケールを設定（重力の強さ）
	float GravityScale;

	// 力の強さ（スケール）
	float ForceScale;

	// 力を加える方向
	FVector ForceDirection;

	// 前回のオブジェクトの位置（移動を比較するため）
	FVector PreviousPosition;

	// 物理シミュレーションのタイマー（力の適用や時間ベースのロジックに使用）
	float Timer;

	// 重力を加えるかどうかのフラグ
	UPROPERTY(EditAnywhere)
	bool bShouldApplyGravity;

	// スイープ衝突判定を使うかどうかのフラグ
	UPROPERTY(EditAnywhere)
	bool bIsSweep;

	// 物理計算が有効かどうかのフラグ
	UPROPERTY(EditAnywhere)
	bool bIsPhysicsEnabled;

	bool hoge = false;
		
};
