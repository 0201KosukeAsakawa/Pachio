// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/State/ColorPainter.h"
#include "Interface/ColorFilterInterface.h"

void AColorPainter::BeginPlay()
{
    Super::BeginPlay();
    Init();
}

void AColorPainter::Init()
{
    // 初期化処理（必要に応じて）
}

void AColorPainter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (PatrolPoints.Num() == 0) return;

    FVector CurrentLocation = GetActorLocation();
    FVector TargetLocation = PatrolPoints[CurrentPatrolIndex];

    FVector Direction = (TargetLocation - CurrentLocation).GetSafeNormal();
    FVector NewLocation = CurrentLocation + Direction * MoveSpeed * DeltaTime;

    if (FVector::Dist(NewLocation, TargetLocation) <= AcceptanceRadius)
    {
        CurrentPatrolIndex = (CurrentPatrolIndex + 1) % PatrolPoints.Num();
    }
    else
    {
        SetActorLocation(NewLocation);
    }
}
