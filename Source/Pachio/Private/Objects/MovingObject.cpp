// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/MovingObject.h"
#include "Sound/SoundManager.h"
#include "ColorUtilityLibrary.h"
#include "Components/BoxComponent.h"
#include "Components/Color/ObjectColorComponent.h"
#include "Manager/LevelManager.h"

// Sets default values
UMoveOnColorComponent::UMoveOnColorComponent():
                                MoveDuration(DEFAULT_DURATION)
                                ,ElapsedTime(0.f)
{
    PrimaryComponentTick.bCanEverTick = true;
    FootTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("FootTrigger"));
    FootTrigger->SetupAttachment(this);

    FootTrigger->SetGenerateOverlapEvents(true);
    FootTrigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    FootTrigger->SetCollisionResponseToAllChannels(ECR_Ignore);
    FootTrigger->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

    FootTrigger->OnComponentBeginOverlap.AddDynamic(this, &UMoveOnColorComponent::OnFootBeginOverlap);
    FootTrigger->OnComponentEndOverlap.AddDynamic(this, &UMoveOnColorComponent::OnFootEndOverlap);
}


void UMoveOnColorComponent::Initialize()
{
    UObjectColorComponent::Initialize();
    TargetLocation = OffLocation;
}

void UMoveOnColorComponent::TickComponent(
    float DeltaTime,
    ELevelTick TickType,
    FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (!bIsMoving)
    {
        return;
    }

    if (MoveDuration <= KINDA_SMALL_NUMBER)
    {
        GetOwner()->SetActorLocation(TargetLocation);
        bIsMoving = false;
        return;
    }

    ElapsedTime += DeltaTime;
    float Alpha = FMath::Clamp(ElapsedTime / MoveDuration, 0.0f, 1.0f);

    FVector CurrentLocation = GetOwner()->GetActorLocation();
    FVector NewLocation = FMath::Lerp(StartLocation, TargetLocation, Alpha);
    FVector DeltaMove = NewLocation - CurrentLocation;

    GetOwner()->SetActorLocation(NewLocation);

    for (AActor* ActorOnTop : AttachedActors)
    {
        if (ActorOnTop)
        {
            ActorOnTop->AddActorWorldOffset(DeltaMove, true);
        }
    }

    for (AActor* ChildActor : Child)
    {
        if (ChildActor)
        {
            ChildActor->AddActorWorldOffset(DeltaMove, true);
        }
    }

    if (Alpha >= 1.0f)
    {
        bIsMoving = false;
    }
}


void UMoveOnColorComponent::ActivateDirect(const FLinearColor& InColor)
{
    StartLocation = GetOwner()->GetActorLocation();
    ElapsedTime = 0.0f;

    if (UColorUtilityLibrary::IsHueSimilar(InColor,CurrentColor))
    {
        TargetLocation = OnLocation;
    }
    else
    {
        TargetLocation = OffLocation;
    }

    bIsMoving = true;
}


void UMoveOnColorComponent::OnFootBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
    bool bFromSweep, const FHitResult& SweepResult)
{
    if (OtherComp && OtherComp->ComponentHasTag(TEXT("Interaction")))
        return;

    if (OtherActor && OtherActor != GetOwner() && !AttachedActors.Contains(OtherActor))
    {
        AttachedActors.Add(OtherActor);
        UE_LOG(LogTemp, Log, TEXT("Added actor on top: %s"), *OtherActor->GetName());
    }
}

void UMoveOnColorComponent::OnFootEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
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