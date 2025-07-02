// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/State/ColorCleaner.h"
#include "Engine/World.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Interface/ColorFilterInterface.h"

AColorCleaner::AColorCleaner()
{
    PrimaryActorTick.bCanEverTick = true;
}

void AColorCleaner::BeginPlay()
{
    Super::BeginPlay();
    StartLocation = GetActorLocation();
    Init();
}

void AColorCleaner::Init()
{
    // 任意の初期化
}

void AColorCleaner::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // 範囲の可視化（デバッグ用）
    FVector Center = StartLocation + (SearchAreaMin + SearchAreaMax) * 0.5f;
    FVector Extent = (SearchAreaMax - SearchAreaMin) * 0.5f;
    DrawDebugBox(GetWorld(), Center, Extent, FColor::Green, false, 0.1f);

    if (TargetActor)
    {
        MoveTowards(TargetActor->GetActorLocation(), DeltaTime);
    }
    else
    {
        AActor* Found = FindTarget();
        if (Found)
        {
            TargetActor = Found;
        }
        else
        {
            Wander(DeltaTime);
        }
    }
}

AActor* AColorCleaner::FindTarget()
{
    FVector AbsMin = StartLocation + SearchAreaMin;
    FVector AbsMax = StartLocation + SearchAreaMax;
    FVector Center = (AbsMin + AbsMax) * 0.5f;
    FVector Extent = (AbsMax - AbsMin) * 0.5f;

    TArray<FHitResult> Hits;
    FCollisionShape Shape = FCollisionShape::MakeBox(Extent);

    GetWorld()->SweepMultiByChannel(
        Hits,
        Center,
        Center,
        FQuat::Identity,
        ECC_WorldDynamic,
        Shape
    );

    for (const auto& Hit : Hits)
    {
        AActor* HitActor = Hit.GetActor();
        if (!HitActor) continue;

        if (Cast<IColorReactiveInterface>(HitActor))
        {
            return HitActor;
        }

    }

    return nullptr;
}

void AColorCleaner::Wander(float DeltaTime)
{
    if (WanderCooldown > 0.f)
    {
        WanderCooldown -= DeltaTime;

        if (!bIsIdle)
        {
            FVector NewLocation = GetActorLocation() + WanderDirection * MoveSpeed * DeltaTime;
            if (IsInsideMoveRange(NewLocation))
            {
                SetActorLocation(NewLocation);
            }
        }

        return;
    }

    int32 Choice = FMath::RandRange(0, 4);
    switch (Choice)
    {
    case 0: WanderDirection = FVector::ForwardVector;  bIsIdle = false; break;
    case 1: WanderDirection = -FVector::ForwardVector; bIsIdle = false; break;
    case 2: WanderDirection = FVector::RightVector;    bIsIdle = false; break;
    case 3: WanderDirection = -FVector::RightVector;   bIsIdle = false; break;
    case 4: WanderDirection = FVector::ZeroVector;     bIsIdle = true;  break;
    }

    WanderCooldown = FMath::FRandRange(1.0f, 3.0f);
}

void AColorCleaner::MoveTowards(const FVector& Destination, float DeltaTime)
{
    FVector Direction = (Destination - GetActorLocation()).GetSafeNormal();
    FVector NewLocation = GetActorLocation() + Direction * MoveSpeed * DeltaTime;

    if (FVector::Dist(NewLocation, Destination) < 50.f)
    {
        SetActorLocation(Destination);
        TargetActor = nullptr;
    }
    else if (IsInsideMoveRange(NewLocation))
    {
        SetActorLocation(NewLocation);
    }
    else
    {
        TargetActor = nullptr;
    }
}

bool AColorCleaner::IsInsideMoveRange(const FVector& Point) const
{
    FVector AbsMin = StartLocation + SearchAreaMin;
    FVector AbsMax = StartLocation + SearchAreaMax;

    return Point.X >= AbsMin.X && Point.X <= AbsMax.X &&
        Point.Y >= AbsMin.Y && Point.Y <= AbsMax.Y &&
        Point.Z >= AbsMin.Z && Point.Z <= AbsMax.Z;
}

void AColorCleaner::Overlap(AActor* OtherActor)
{
    if (!OtherActor || OtherActor == this) return;

    IColorReactiveInterface* Interface = Cast<IColorReactiveInterface>(OtherActor);
    if (Interface && !Interface->IsColorLock())
    {
        Interface->ResetColor();
    }
}