//------------------------------------------
// ファイル名：ThrowingBase.h
// 処理内容：投げられるもの基底クラス
//------------------------------------------
// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ThrowingBase.generated.h"

//前方宣言
class UPhysicsCalculator;

UCLASS()
class PACHIO_API AThrowingBase : public AActor
{
	GENERATED_BODY()

protected:
	// ゲーム開始時に呼ばれる
	virtual void BeginPlay() override;

public:
	//Function to throw things
	//第1引数：ものを投げる向き
	//第2引数：投げる際にかける力
	virtual void ThrowObject(FVector AimDirection, float Impulse);

	//A function that determines the Object to throw.
	//第1引数：投げるものの詳細
	virtual void SetThrownObject(AThrowingBase*);

	//Function to set direction and force
	virtual void SetDirection(FVector newDirection) { Direction = newDirection; }
	virtual void SetForce(float newForce) { Force = newForce; }

protected:
	UPROPERTY()
	UPhysicsCalculator* Physics;

	UPROPERTY()
	FVector Direction = FVector(0.0f, 30.0f, 0.0f).GetSafeNormal();		//向き

	UPROPERTY()
	float Force = 1.0f;		//力

	UPROPERTY()
	bool bHit = false;		//地面に設置したかどうかの判定
};
