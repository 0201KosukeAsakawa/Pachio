//------------------------------------------
// ファイル名：ThrowComponent.cpp
// 処理内容：投げる処理
//------------------------------------------
// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/ThrowComponent.h"
#include "Objects/ThrowableBase.h"
#include "Engine/World.h"


//ものを投げる関数
void UThrowComponent::ThrowObject(FVector direction, float force)
{
	if (!ThrownObjectPtr)
		return;

	ThrownObjectPtr->ThrowObject(direction, force);
}

//投げるObjectを決定する関数
void UThrowComponent::SelectThrownObject(UThrowableBase* thrownObjectPtr)
{
	if (thrownObjectPtr == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed SelectThrownObject Function"));
		return;
	}

	//引数thrownObjectPtrを変数ThrownObjectPtrに代入
	ThrownObjectPtr = thrownObjectPtr;

}


