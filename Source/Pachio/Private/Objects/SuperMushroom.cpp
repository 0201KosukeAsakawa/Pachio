// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/SuperMushroom.h"
#include "Components/SphereComponent.h"
#include "Components/PhysicsCalculator.h"
#include "Components/MoveComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Player/PlayerCharacter.h"


ASuperMushroom::ASuperMushroom()
{
    PrimaryActorTick.bCanEverTick = true;

    // スフィアコリジョンで重なった際にイベントをバインド
    USphereComponent* CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));
    RootComponent = CollisionComponent;
    CollisionComponent->InitSphereRadius(50.0f);
    CollisionComponent->SetCollisionProfileName(TEXT("OverlapAllDynamic"));

    // OnCollected にバインド
    CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &ASuperMushroom::OnCollected);
}

void ASuperMushroom::BeginPlay()
{
    Super::BeginPlay();
    moveComp = NewObject<UMoveComponent>(this);
    physics = NewObject<UPhysicsCalculator>(this);
    moveComp->Init(this);
}

void ASuperMushroom::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    if (!moveComp)
        return;

    moveComp->Movement(DeltaTime);
    physics->AddGravity();
}

void ASuperMushroom::OnCollected(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    // 他のアクターがマリオかどうか確認
    if (APlayerCharacter* Mario = Cast<APlayerCharacter>(OtherActor))
    {
        // マリオにパワーアップ通知
       // Mario->RequestPowerUp(EPowerUpType::SuperMushroom);

        // エフェクトを表示
        if (PowerUpEffect)
        {
            PowerUpEffect->Activate();
        }

        // アイテムを消す
        Destroy();
    }
}

void ASuperMushroom::SetDirection(FVector direction)
{
    if (!moveComp)
        return;

    moveComp->SetDirection(direction);
}

