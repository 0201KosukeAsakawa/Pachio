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

}

void AColorPainter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (PatrolPoints.Num() == 0) return;

    FVector CurrentLocation = GetActorLocation();
    FVector TargetLocation = PatrolPoints[CurrentPatrolIndex];

    FVector Direction = (TargetLocation - CurrentLocation).GetSafeNormal();
    FVector NewLocation = CurrentLocation + Direction * MoveSpeed * DeltaTime;

    // �ړI�n�ɏ\���߂Â����玟�̃|�C���g��
    if (FVector::Dist(NewLocation, TargetLocation) <= AcceptanceRadius)
    {
        CurrentPatrolIndex = (CurrentPatrolIndex + 1) % PatrolPoints.Num();
    }
    else
    {
        SetActorLocation(NewLocation);
    }
}


void AColorPainter::Overlap(AActor* OtherActor)
{
    if (!OtherActor || OtherActor == this) return;

    IColorReactiveInterface* Interface = Cast<IColorReactiveInterface>(OtherActor);
    if (Interface)
    {
        Interface->SetColor(PainterColor, Match);
    }
}