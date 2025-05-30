//------------------------------------------
// ファイル名：ThrowingBase.cpp
// 処理内容：投げられるもの基底クラス
//------------------------------------------
// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/ThrowingBase.h"

void AThrowingBase::BeginPlay()
{
	Super::BeginPlay(); // 親クラスのBeginPlayを呼び出す
}


//Function to throw things
//情報を渡す関数
void AThrowingBase::ThrowObject(FVector AimDirection, float Impulse)
{		
}

//A function that determines the Object to throw.
void AThrowingBase::SetThrownObject(AThrowingBase* thrownObjectPtr)
{
}
