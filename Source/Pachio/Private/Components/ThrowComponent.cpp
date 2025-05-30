//------------------------------------------
// ファイル名：ThrowComponent.cpp
// 処理内容：投げる処理
//------------------------------------------
// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/ThrowComponent.h"
#include "Objects/ThrowingBase.h"
#include "Engine/World.h"

//初期設定
void UThrowComponent::Init()
{
	mOwner = GetOwner();
	
}

void UThrowComponent::BeginPlay()
{
	Super::BeginPlay(); // 親クラスのBeginPlayを呼び出す

	Init();
}

//ものを投げる関数
void UThrowComponent::ThrowObject(FVector direction, float force)
{
	//クラスの設定があるかを確認
	if (!IsValid(ThrownObjectClass))
	{
		UE_LOG(LogTemp, Warning, TEXT("No object to spawn!"));
		return;
	}

	// スポーン位置と回転を決定
	FVector SpawnLocation = mOwner->GetActorLocation() + FVector(0, 50, 0);  //位置
	FRotator SpawnRotation = FRotator::ZeroRotator;  //回転位置

	// UWorld を取得
	UWorld* World = GetWorld();
	if (!IsValid(World)) { 
		return; 
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = nullptr;
	SpawnParams.Instigator = 0;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	//投げるオブジェクトをスポーン
	AThrowingBase* SpawnedActor = World->SpawnActor<AThrowingBase>(ThrownObjectClass, SpawnLocation, SpawnRotation, SpawnParams);
	if (!IsValid(SpawnedActor))
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to spawn the object."));
		return;
	}

	//スポーンアクタの向きと加速度を渡す
	SpawnedActor->SetDirection(FVector(0.0f, 90.0f, -50.0f).GetSafeNormal());
	SpawnedActor->SetForce(30.0f);
}

//投げるObjectを決定する関数
void UThrowComponent::SetThrownObject(TSubclassOf<AThrowingBase> thrownObjectClass)
{
	if (!IsValid(thrownObjectClass))
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed SelectThrownObject Function"));
		return;
	}

	//引数thrownObjectPtrを変数ThrownObjectPtrに代入
	ThrownObjectClass = thrownObjectClass;

}
