// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/ItemBase.h"
#include "Objects/SuperMushroom.h"
#include "Components/SphereComponent.h"
#include "Interface/ItemEffectSource.h"

// Sets default values
AItemBase::AItemBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    // スフィアコリジョンで重なった際にイベントをバインド
    USphereComponent* CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));
    RootComponent = CollisionComponent;
    CollisionComponent->InitSphereRadius(50.0f);
    CollisionComponent->SetCollisionProfileName(TEXT("OverlapAllDynamic"));

    // OnCollected にバインド
    CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &AItemBase::OnCollected);
}

// Called when the game starts or when spawned
void AItemBase::BeginPlay()
{
    Super::BeginPlay();
    my = NewObject<USuperMushroomComponent>(this);
    if (my)
        my->Init();
}

// Called every frame
void AItemBase::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    if (my)
        my->Update(DeltaTime);
}

void AItemBase::OnCollected(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (my)
    {
        my->OnCollected(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
    }
}

