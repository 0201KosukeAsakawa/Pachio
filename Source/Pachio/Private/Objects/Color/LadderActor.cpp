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

FRotator ALadderActor::GetRotationForActorToFaceThis(AActor* OtherActor) const
{
	if (!OtherActor)
	{
		return FRotator::ZeroRotator;
	}

	FVector ALocation = GetActorLocation();
	FVector OtherLocation = OtherActor->GetActorLocation();

	FVector Forward = GetActorForwardVector();
	Forward.Z = 0;
	Forward.Normalize();

	// 自分からOtherActorへのベクトル
	FVector ToOther = (OtherLocation - ALocation);
	ToOther.Z = 0;
	ToOther.Normalize();

	float Dot = FVector::DotProduct(Forward, ToOther);


	if (Dot >= 0.f)
	{
		// OtherActorは自分の前にいる → そのまま（回転不要）
		return FRotator(0.f, 0.f, 0.f);
	}
	else
	{
		// OtherActorは後ろにいる → 180度回転させる
		return FRotator(0.f, 180.f, 0.f);
	}
}
