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
void AInGameController::ReturnToOriginalPlayer()
{
    if (OriginalPawn)
    {
        Possess(OriginalPawn);
    }
}

AActor* AInGameController::FindPossessableObject()
{
    FVector Start;
    FRotator Rotation;
    GetPlayerViewPoint(Start, Rotation);

    FVector End = Start + Rotation.Vector() * DetectionDistance;
    const FQuat TraceRotation = FQuat::Identity;

    // BoxHalfSize‚ð10”{‚ÉŠg‘å
    const FVector EnlargedBoxHalfSize = BoxHalfSize * 100000.0f;
    const FCollisionShape Box = FCollisionShape::MakeBox(EnlargedBoxHalfSize);

    FCollisionQueryParams Params;
    Params.AddIgnoredActor(GetPawn());

    TArray<FHitResult> HitResults;
    bool bHit = GetWorld()->SweepMultiByChannel(
        HitResults,
        Start,
        End,
        TraceRotation,
        ECC_Visibility,
        Box,
        Params
    );

    if (bHit)
    {
        for (const FHitResult& Hit : HitResults)
        {
            if (Hit.GetActor() && Hit.GetActor()->ActorHasTag(TEXT("Possessable")))
            {
                return Hit.GetActor();
            }
        }
    }

    return nullptr;
}
