// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/LadderActor.h"
#include "Components/BoxComponent.h"

// Sets default values
ALadderActor::ALadderActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	LadderVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComonent"));
}

// Called when the game starts or when spawned
void ALadderActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ALadderActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

FVector ALadderActor::GetTopWorldPosition() const
{
	if (LadderVolume)
	{
		FVector Origin, Extent = FVector(0, 0, 0);
		Extent = LadderVolume->GetScaledBoxExtent();
		Origin = LadderVolume->GetComponentLocation();
		return Origin + FVector(0, 0, Extent.Z);
	}
	return GetActorLocation();
}

FVector ALadderActor::GetBottomWorldPosition() const
{
	if (LadderVolume)
	{
		FVector Origin, Extent = FVector(0,0,0);
		Extent = LadderVolume->GetScaledBoxExtent();
		Origin = LadderVolume->GetComponentLocation();
		return Origin - FVector(0, 0, Extent.Z);
	}
	return GetActorLocation();
}
