// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/RespawnComponent.h"
// コンポーネントのコンストラクタ
URespawnComponent::URespawnComponent()
{
	// 毎フレームTickを有効化
	PrimaryComponentTick.bCanEverTick = true;
}

// ゲーム開始時の初期化処理
void URespawnComponent::BeginPlay()
{
	Super::BeginPlay();

	// 所持するActor（Owner）の初期位置を記録（リスポーン位置）
	if (AActor* Owner = GetOwner())
	{
		respawnPosition = Owner->GetActorLocation();
	}
}

// 毎フレーム呼ばれるTick関数（今は未使用）
void URespawnComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

// Ownerをリスポーン位置に移動させる関数
void URespawnComponent::RespawnOwner()
{
	if (AActor* Owner = GetOwner())
	{
		Owner->SetActorLocation(respawnPosition);
	}
}

// リスポーン位置を更新する関数
void URespawnComponent::UpdateRespawnPosition(FVector newPosition)
{
	respawnPosition = newPosition;
}
