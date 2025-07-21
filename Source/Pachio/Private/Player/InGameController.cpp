// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/InGameController.h"
#include "Objects/ControllableObjectBase.h"

void AInGameController::SetupInputComponent()
{
	Super::SetupInputComponent();
	InputComponent->BindAction("TogglePossess", IE_Pressed, this, &AInGameController::TogglePossession);

}

void AInGameController::TogglePossession()
{
    if (bIsPossessing && OriginalPawn)
    {
        Possess(OriginalPawn);
        bIsPossessing = false;
        return;
    }

    AActor* HitActor = FindPossessableObject();
    if (AControllableObjectBase* Target = Cast<AControllableObjectBase>(HitActor))
    {
        OriginalPawn = GetPawn();
        Possess(Cast<APawn>(Target));
        bIsPossessing = true;
    }
}

AActor* AInGameController::FindPossessableObject()
{
    FVector Start;
    FRotator Rotation;
    GetPlayerViewPoint(Start, Rotation);

    FVector End = Start + Rotation.Vector() * DetectionDistance;
    FHitResult Hit;

    const FQuat TraceRotation = FQuat::Identity;
    const FCollisionShape Box = FCollisionShape::MakeBox(BoxHalfSize);

    FCollisionQueryParams Params;
    Params.AddIgnoredActor(GetPawn());

    bool bHit = GetWorld()->SweepSingleByChannel(
        Hit,
        Start,
        End,
        TraceRotation,
        ECC_Visibility,
        Box,
        Params
    );

    if (bHit && Hit.GetActor() && Hit.GetActor()->ActorHasTag("Possessable"))
    {
        return Hit.GetActor();
    }

    return nullptr;
}
