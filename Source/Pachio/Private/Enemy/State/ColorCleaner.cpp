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

    // デバッグ：探索範囲の可視化
    FVector Center = StartLocation + (SearchAreaMin + SearchAreaMax) * 0.5f;
    FVector Extent = (SearchAreaMax - SearchAreaMin) * 0.5f;
    DrawDebugBox(GetWorld(), Center, Extent, FColor::Green, false, 0.1f);

    if (bIsRotating)
    {
        RotationElapsed += DeltaTime;
        float Alpha = FMath::Clamp(RotationElapsed / RotationDuration, 0.f, 1.f);

        FRotator CurrentRotation = GetActorRotation();
        FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaTime, 10.0f);
        SetActorRotation(NewRotation);

        if (Alpha >= 1.0f || CurrentRotation.Equals(TargetRotation, 1.0f))
        {
            bIsRotating = false;
            RotationElapsed = 0.f;

            if (bHasPendingMove)
            {
                bHasPendingMove = false;
                MoveTowards(PendingMoveDestination, DeltaTime);
            }
        }
        return;
    }

    if (TargetActor)
    {
       /* if (TargetActor->IsHidden() || !IsInsideMoveRange(TargetActor->GetActorLocation()))
        {
            TargetActor = nullptr;
            return;
        }

        IColorReactiveInterface* Interface = Cast<IColorReactiveInterface>(TargetActor);
        if (Interface && (Interface->IsColorModifiable() || Interface->IsColorChange()))
        {
            TargetActor = nullptr;
            return;
        }*/

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
 /*   FVector AbsMin = StartLocation + SearchAreaMin;
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
        IColorReactiveInterface* Interface = Cast<IColorReactiveInterface>(HitActor);
        if (Interface && !Interface->IsColorModifiable() && !Interface->IsColorChange())
        {
            return HitActor;
        }
    }*/

    return nullptr;
}

void AColorCleaner::Wander(float DeltaTime)
{
    if (WanderCooldown > 0.f)
    {
        WanderCooldown -= DeltaTime;

        if (!bIsIdle)
        {
            FVector Forward = GetActorForwardVector();
            FVector NewLocation = GetActorLocation() + Forward * MoveSpeed * DeltaTime;

            FHitResult HitResult;
            FVector Start = GetActorLocation();
            FVector End = Start + Forward * 100.f;

            FCollisionQueryParams Params;
            Params.AddIgnoredActor(this);

            bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_WorldStatic, Params);

            if (bHit || !IsInsideMoveRange(NewLocation) || IsCliffAhead(Forward))
            {
                WanderCooldown = 0.f;
                return;
            }

            SetActorLocation(NewLocation);
        }

        return;
    }

    TArray<FVector> LocalDirections = {
        GetActorForwardVector(),
        -GetActorForwardVector(),
        GetActorRightVector(),
        -GetActorRightVector()
    };

    for (int i = 0; i < LocalDirections.Num(); ++i)
    {
        int32 SwapIdx = FMath::RandRange(i, LocalDirections.Num() - 1);
        LocalDirections.Swap(i, SwapIdx);
    }

    for (const FVector& Dir : LocalDirections)
    {
        FVector Start = GetActorLocation();
        FVector End = Start + Dir * 100.f;

        FHitResult HitResult;
        FCollisionQueryParams Params;
        Params.AddIgnoredActor(this);

        bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_WorldStatic, Params);

        if (!bHit && !IsCliffAhead(Dir))
        {
            TargetRotation = Dir.Rotation();
            bIsRotating = true;
            RotationElapsed = 0.f;

            bIsIdle = false;
            WanderCooldown = FMath::FRandRange(1.0f, 3.0f);
            return;
        }
    }

    bIsIdle = true;
    WanderCooldown = FMath::FRandRange(1.0f, 2.0f);
}

void AColorCleaner::MoveTowards(const FVector& Destination, float DeltaTime)
{
    FVector CurrentLocation = GetActorLocation();
    FVector Direction = (Destination - CurrentLocation).GetSafeNormal();

    const float AngleThreshold = 5.0f;
    float AngleDiff = FMath::Abs(FRotator::NormalizeAxis(Direction.Rotation().Yaw - GetActorRotation().Yaw));

    if (AngleDiff > AngleThreshold)
    {
        TargetRotation = Direction.Rotation();
        bIsRotating = true;
        RotationElapsed = 0.f;

        PendingMoveDestination = Destination;
        bHasPendingMove = true;
        return;
    }

    FVector DesiredLocation = CurrentLocation + Direction * MoveSpeed * DeltaTime;

    FHitResult HitResult;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(this);

    bool bHit = GetWorld()->LineTraceSingleByChannel(
        HitResult,
        CurrentLocation,
        DesiredLocation,
        ECC_WorldStatic,
        Params
    );

    if (bHit || IsCliffAhead(Direction))
    {
        FVector StopLocation = bHit ? HitResult.ImpactPoint - Direction * 5.0f : GetActorLocation();
        SetActorLocation(StopLocation);

        TargetActor = nullptr;
        WanderCooldown = 0.f;
        return;
    }

    if (FVector::Dist(DesiredLocation, Destination) < 50.f)
    {
        SetActorLocation(Destination);
        TargetActor = nullptr;
    }
    else if (IsInsideMoveRange(DesiredLocation))
    {
        SetActorLocation(DesiredLocation);
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

bool AColorCleaner::IsCliffAhead(const FVector& Direction)
{
    FVector Start = GetActorLocation() + Direction * 50.0f;
    FVector End = Start - FVector(0, 0, 100.0f);

    FHitResult HitResult;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(this);

    return !GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, Params);
}

void AColorCleaner::Overlap(AActor* OtherActor)
{
    //if (!OtherActor || OtherActor == this) return;

    //IColorReactiveInterface* Interface = Cast<IColorReactiveInterface>(OtherActor);
    //if (Interface && !Interface->IsColorModifiable() && !Interface->IsColorChange())
    //{
    //    Interface->ResetColor();
    //}
}
