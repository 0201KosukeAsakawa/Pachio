//------------------------------------------
// ファイル名：ThrowableBase.h
// 処理内容：投げられるもの純粋クラス
//------------------------------------------
// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ThrowableBase.generated.h"

//前方宣言
class UPhysicsCalculator;

UCLASS()
class PACHIO_API UThrowableBase : public UObject
{
	GENERATED_BODY()
	
public:

	//ものを投げる関数
	//第1引数：ものを投げる向き
	//第2引数：投げる際にかける力
	virtual void ThrowObject(FVector, float);

	//投げるObjectを決定する関数
	//第1引数：投げるものの詳細
	virtual void SelectThrownObject(UPhysicsCalculator*);


protected:
	UPROPERTY()
	UPhysicsCalculator* Physics;

};
