// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/InvincibilityComponent.h"
#include "FunctionLibrary.h"

// Sets default values for this component's properties
UInvincibilityComponent::UInvincibilityComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UInvincibilityComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UInvincibilityComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (!bIsInvincible)
		return;
	UpdateInvincible(DeltaTime);
}

void UInvincibilityComponent::StartInvincible()
{
	// 無敵時間開始
	bIsInvincible = true;
	InvincibleTime = MaxInvincibleTime;
}

void UInvincibilityComponent::ToggleVisibility()
{
	UStaticMeshComponent* pMesh = UFunctionLibrary::FindComponentByName<UStaticMeshComponent>(GetOwner(), "StaticMesh");
	if (pMesh == nullptr)
		return;

	if (bIsVisible)
	{
		pMesh->SetVisibility(false);
	}
	else
	{
		pMesh->SetVisibility(true);
	}

	// 状態を反転
	bIsVisible = !bIsVisible;
}

void UInvincibilityComponent::UpdateInvincible(float DeltaTime)
{
	UStaticMeshComponent* pMesh = UFunctionLibrary::FindComponentByName<UStaticMeshComponent>(GetOwner(), "StaticMesh");
	if (pMesh == nullptr)
		return;

	// 無敵時間を減少させる
	InvincibleTime -= DeltaTime;

	if (InvincibleTime <= 0.0f)
	{
		bIsInvincible = false; // 無敵時間終了
		GetWorld()->GetTimerManager().ClearTimer(BlinkTimerHandle);  // タイマーの停止
		pMesh->SetVisibility(true);  // 最後にメッシュを表示状態に戻す
	}
	else
	{
		// 点滅の処理
		if (!GetWorld()->GetTimerManager().IsTimerActive(BlinkTimerHandle))
		{
			// タイマーを設定して、定期的に点滅させる
			GetWorld()->GetTimerManager().SetTimer(BlinkTimerHandle, this, &UInvincibilityComponent::ToggleVisibility, 0.1f, true);
		}
	}
}

