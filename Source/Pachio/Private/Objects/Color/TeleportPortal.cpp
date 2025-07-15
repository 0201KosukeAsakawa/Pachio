// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/Color/TeleportPortal.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "Components/ColorReactiveComponent.h"
ATeleportPortal::ATeleportPortal()
{
    PrimaryActorTick.bCanEverTick = false;

    CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
    RootComponent = CollisionBox;
    CollisionBox->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
    CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &ATeleportPortal::OnOverlapBegin);
}

void ATeleportPortal::BeginPlay()
{
    Super::BeginPlay();
}

void ATeleportPortal::Init()
{
    AColorReactiveObject::Init();
    CurrentTargetPortal = PrimaryDestination;
}

void ATeleportPortal::ColorAction(const FLinearColor InColor)
{
    bool b = ColorReactiveComponent->IsColorMatch(InColor);
    if (b)
    {
        CurrentTargetPortal = AlternatePortal;
    }
    else
    {
        bool c = ColorReactiveComponent->IsColorMatch(InColor, SecondColor);
        if (c)
            CurrentTargetPortal = PrimaryDestination;
    }
}

void ATeleportPortal::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
    bool bFromSweep, const FHitResult& SweepResult)
{
    // 必須チェック
    if (!OtherActor || !CurrentTargetPortal || OtherActor == this)
        return;

    UWorld* World = GetWorld();
    if (!World)
        return;

    float CurrentTime = World->GetTimeSeconds();

    // クールダウン中なら処理しない
    float* LastTimePtr = LastTeleportTime.Find(OtherActor);
    if (LastTimePtr && (CurrentTime - *LastTimePtr < TeleportCooldown))
    {
        return;
    }

    // 安全にテレポート処理
    FVector TargetLocation = CurrentTargetPortal->GetActorLocation();
    LastTeleportTime.Add(OtherActor, CurrentTime);

    // クールダウン時刻を記録（双方向）
    CurrentTargetPortal->LastTeleportTime.Add(OtherActor, CurrentTime);

    OtherActor->SetActorLocation(TargetLocation);

}
