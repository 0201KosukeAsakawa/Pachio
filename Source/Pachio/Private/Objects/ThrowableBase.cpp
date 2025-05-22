//------------------------------------------
// ファイル名：ThrowableBase.cpp
// 処理内容：投げられるもの純粋クラス
//------------------------------------------
// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/ThrowableBase.h"
#include "Components/PhysicsCalculator.h"

//ものを投げる関数
void UThrowableBase::ThrowObject(FVector direction, float force)
{		//加える力の設定
	Physics->AddForce(direction, force);
	UE_LOG(LogTemp, Warning, TEXT("Succesed PhysicsAddForce"));
}

//投げるObjectを決定する関数
void UThrowableBase::SelectThrownObject(UPhysicsCalculator*)
{
}
