// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/MovingObject.h"
#include "Sound/SoundManager.h"
#include "Components/BoxComponent.h"
#include "Components/Color/ColorConfigurator.h"
#include"Manager/LevelManager.h"

// Sets default values
AMovingObject::AMovingObject()
{
    PrimaryActorTick.bCanEverTick = true;

    FootTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("FootTrigger"));
    RootComponent = FootTrigger;

    FootTrigger->SetGenerateOverlapEvents(true);
    FootTrigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    FootTrigger->SetCollisionResponseToAllChannels(ECR_Ignore);
    FootTrigger->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

    FootTrigger->OnComponentBeginOverlap.AddDynamic(this, &AMovingObject::OnFootBeginOverlap);
    FootTrigger->OnComponentEndOverlap.AddDynamic(this, &AMovingObject::OnFootEndOverlap);
}


void AMovingObject::Init()
{
    AColorReactiveObject::Init();
    TargetLocation = OffLocation;
}

void AMovingObject::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (bIsMoving)
    {
        FVector CurrentLocation = GetActorLocation();
        FVector Direction = (TargetLocation - CurrentLocation).GetSafeNormal();
        float Speed = 400.f;

        FVector NewLocation = CurrentLocation + Direction * Speed * DeltaTime;
        FVector DeltaMove = NewLocation - CurrentLocation;

        if (FVector::Dist(NewLocation, TargetLocation) < 10.f)
        {
            DeltaMove = TargetLocation - CurrentLocation;
            NewLocation = TargetLocation;
            bIsMoving = false;
        }

        SetActorLocation(NewLocation);
        TArray < AActor*>Target = AttachedActors;
        // ����Ă���A�N�^�[��ꏏ�ɓ�����
        for (AActor* ActorOnTop : Target)
        {
            if (ActorOnTop)
            {
                ActorOnTop->AddActorWorldOffset(DeltaMove);
            }
        }

        // �q�I�u�W�F�N�g��������ꍇ�i���������j
        for (AActor* ChildActor : Child)
        {
            if (ChildActor)
            {
                ChildActor->AddActorWorldOffset(DeltaMove);
            }
        }
    }
}


void AMovingObject::ColorAction(FLinearColor InColor, FEffectMatchResult result)
{
    AColorReactiveObject::ColorAction(InColor, result);

    if (ColorConfigurator->IsColorMatch())
    {
        TargetLocation = OffLocation;
    }
    else
    {
        TargetLocation = OnLocation;
    }
    bIsMoving = true;
}

void AMovingObject::OnFootBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
    bool bFromSweep, const FHitResult& SweepResult)
{
    if (OtherComp && OtherComp->ComponentHasTag(TEXT("Interaction")))
        return;

    if (OtherActor && OtherActor != this && !AttachedActors.Contains(OtherActor))
    {
        AttachedActors.Add(OtherActor);
        UE_LOG(LogTemp, Log, TEXT("Added actor on top: %s"), *OtherActor->GetName());
    }
}

void AMovingObject::OnFootEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    if (OtherComp && OtherComp->ComponentHasTag(TEXT("Interaction")))
        return;

    if (OtherActor && AttachedActors.Contains(OtherActor))
    {
        AttachedActors.Remove(OtherActor);
        UE_LOG(LogTemp, Log, TEXT("Removed actor from top: %s"), *OtherActor->GetName());
    }
}