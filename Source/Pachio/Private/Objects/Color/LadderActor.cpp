// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/Color/LadderActor.h"
#include "Components/BoxComponent.h"

// Sets default values
ALadderActor::ALadderActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	LadderVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComonent"));
}

void ALadderActor::Init()
{
	AColorReactiveObject::Init(); // 親の Init を呼ぶ
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

FVector ALadderActor::GetFixedPositionForActor( AActor* OtherActor) const
{
	if (!OtherActor) return GetActorLocation();

	FVector ALocation = GetActorLocation();
	FVector OtherLocation = OtherActor->GetActorLocation();

	// Aから見て左右どちらにいるかを判定
	FVector ARight = GetActorRightVector();
	FVector DirToOther = (OtherLocation - ALocation).GetSafeNormal();
	float Dot = FVector::DotProduct(DirToOther, ARight);

	FVector FixedLocation = OtherLocation;

	if (Dot > 0)
	{
		FixedLocation.Y = ALocation.Y + Offset; // 右側
	}
	else
	{
		FixedLocation.Y = ALocation.Y - Offset; // 左側
	}

	return FixedLocation;
}