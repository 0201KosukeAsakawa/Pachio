//------------------------------------------
// ファイル名：Fireball.cpp
// 処理内容：投げる球派生クラス
//------------------------------------------
// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/Fireball.h"
#include "Components/PhysicsCalculator.h"

AFireball::AFireball()
{
	PrimaryActorTick.bCanEverTick = true;
 
	//UPhysicsCalculatorを生成・アタッチを行う
	Physics = CreateDefaultSubobject<UPhysicsCalculator>(TEXT("PhysicsCalculator"));
	if (!IsValid(Physics))
	{
		UE_LOG(LogTemp, Error, TEXT("Physics component not found!"));
	}
}

void AFireball::BeginPlay()
{
	Super::BeginPlay();		//親クラスのBeginPlayを呼び出す

}

// 毎フレーム呼ばれる
void AFireball::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	ThrowObject(Direction, Force);

}

//Function to throw things
void AFireball::ThrowObject(FVector AimDirection, float Impulse)
{
	if (!IsValid(Physics))
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to Not Physics"));
		return;
	}

	//加える力を加える
	Physics->AddForce(Direction, Force, true);
	UE_LOG(LogTemp, Warning, TEXT("Succeeded PhysicsAddForce"));
	
	//重力を加える
	Physics->AddGravity();
	UE_LOG(LogTemp, Warning, TEXT("Succeeded PhysicsAddGravity"));

}