// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/RespawnComponent.h"
URespawnComponent::URespawnComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void URespawnComponent::BeginPlay()
{
	Super::BeginPlay();

	// ゲーム開始時のOwnerの位置を記録
	if (AActor* Owner = GetOwner())
	{
		InitialLocation = Owner->GetActorLocation();
	}
}

void URespawnComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void URespawnComponent::RespawnOwnerAtInitialLocation()
{
	if (AActor* Owner = GetOwner())
	{
		Owner->SetActorLocation(InitialLocation);
	}
}
