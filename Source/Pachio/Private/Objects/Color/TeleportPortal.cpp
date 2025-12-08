// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/Color/TeleportPortal.h"
#include "Components/BoxComponent.h"
#include "Components/Color/ColorReactiveComponent.h"
#include "Components/Color/ObjectColorComponent.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "Manager/LevelManager.h"
#include "Manager/ColorManager.h"

ATeleportPortal::ATeleportPortal():
                                    TeleportCooldown(1.f)
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

void ATeleportPortal::Initialize()
{
    AColorReactiveObject::Initialize();
    CurrentTargetPortal = PrimaryDestination;
    SecondColor = ALevelManager::GetInstance(GetWorld())
        ->GetColorManager()
        ->GetEffectColor(Effect);
}

void ATeleportPortal::ApplyColorWithMatching(const FLinearColor& InColor)
{
    if (!ObjectColorComponent)
        return;

    bool b = UColorUtilityLibrary::IsHueSimilar(InColor, SecondColor);
    if (b)
    {
        if (AlternatePortal)
            CurrentTargetPortal = AlternatePortal;
    }
    else
    {
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

    // タグが一致しない場合は処理しない（空なら全許可）
    if (AllowedTags.Num() > 0)
    {
        bool bMatch = false;
        for (const FName& Tag : AllowedTags)
        {
            if (OtherActor->ActorHasTag(Tag))
            {
                bMatch = true;
                break;
            }
        }

        if (!bMatch)
            return;
    }

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
