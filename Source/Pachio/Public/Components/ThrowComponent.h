//------------------------------------------
// ファイル名：ThrowComponent.h
// 処理内容：投げる処理
//------------------------------------------
// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ThrowComponent.generated.h"

//前方宣言
class UPhysicsCalculator;
class UThrowableBase;

UCLASS(Blueprintable)
class PACHIO_API UThrowComponent : public UObject
{
	GENERATED_BODY()

public:
	//ものを投げる関数
	//第1引数：ものを投げる向き
	//第2引数：投げる際にかける力
	virtual void ThrowObject(FVector Direction, float Force);

	//投げるObjectを決定する関数
	//第1引数：投げるものの詳細
	virtual void SelectThrownObject(UThrowableBase* thrownObjectPtr);

	virtual void SetDirection(FVector direction ,  float force);

protected:
	//投げられるものクラス（ThrowableBase）を持つ変数
	UPROPERTY()
	UThrowableBase* ThrownObjectPtr;
	UPROPERTY()
	UPhysicsCalculator* PhysicsPtr;

};
