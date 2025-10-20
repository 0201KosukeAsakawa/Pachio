// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/MovingObject.h"
#include "Sound/SoundManager.h"
#include "ColorUtilityLibrary.h"
#include "Components/BoxComponent.h"
#include "Components/Color/ObjectColorComponent.h"
#include "Manager/LevelManager.h"

// Sets default values
AMovingObject::AMovingObject():
                                MoveDuration(DEFAULT_DURATION)
                                ,ElapsedTime(0.f)
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


void AMovingObject::Initialize()
{
    AColorReactiveObject::Initialize();
    TargetLocation = OffLocation;
}

void AMovingObject::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (bIsMoving)
    {
        ElapsedTime += DeltaTime;
        float Alpha = FMath::Clamp(ElapsedTime / MoveDuration, 0.0f, 1.0f);

        FVector NewLocation = FMath::Lerp(StartLocation, TargetLocation, Alpha);
        FVector DeltaMove = NewLocation - GetActorLocation();

        SetActorLocation(NewLocation);
        TArray<AActor*> Actors = AttachedActors;
        // 上に乗っているアクターも追従
        for (AActor* ActorOnTop : Actors)
        {
            if (ActorOnTop)
            {
                ActorOnTop->AddActorWorldOffset(DeltaMove,true);
            }
        }

        // 子オブジェクトも追従
        for (AActor* ChildActor : Child)
        {
            if (ChildActor)
            {
                ChildActor->AddActorWorldOffset(DeltaMove,true);
            }
        }

        // 移動完了判定
        if (Alpha >= DEFAULT_DURATION)
        {
            bIsMoving = false;
        }
    }
}

void AMovingObject::ApplyColorWithMatching(const FLinearColor& InColor)
{
    AColorReactiveObject::ApplyColorWithMatching(InColor);

    StartLocation = GetActorLocation();
    ElapsedTime = 0.0f; // 経過時間リセット
    //色の差を求める
    float distance = UColorUtilityLibrary::GetColorRatio(InColor, ObjectColorComponent->GetCurrentColor());
    
    FVector Direction = OnLocation - OffLocation;  // On - Offの差分ベクトル
    if (ObjectColorComponent->HasColorChanged())
    {
        TargetLocation = OffLocation + Direction * distance;  // OffからOnへdistance割合だけ動く
    }
    else
    {
        TargetLocation = OnLocation - Direction * distance;  // OnからOffへdistance割合だけ動く
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