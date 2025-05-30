//------------------------------------------
// ファイル名：Fireball.h
// 処理内容：投げる球派生クラス
//------------------------------------------
// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Objects/ThrowingBase.h"
#include "Fireball.generated.h"


UCLASS(Blueprintable)
class PACHIO_API AFireball : public AThrowingBase
{
	GENERATED_BODY()

	AFireball();

protected:
	// ゲーム開始時に呼ばれる
	virtual void BeginPlay() override;

public:
	// Tick関数のオーバーライド宣言
	virtual void Tick(float DeltaTime) override;

	//Function to throw things
	UFUNCTION(BlueprintCallable, Category = "Throwing")
	virtual void ThrowObject(FVector AimDirection, float Impulse) override;

	//Function to set direction and force
	virtual void SetDirection(FVector newDirection)override { Direction = newDirection; }
	virtual void SetForce(float newForce)override { Force = newForce; }

};
