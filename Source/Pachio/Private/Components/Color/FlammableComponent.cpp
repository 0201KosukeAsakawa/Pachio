// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/Color/FlammableComponent.h"
#include "Components/BoxComponent.h"
#include "Interface/StateControllable.h"

UFlammableComponent::UFlammableComponent()
{
    HitBox = CreateDefaultSubobject<UBoxComponent>(TEXT("FireBox"));
}

void UFlammableComponent::BeginPlay()
{
    Super::BeginPlay();

    if (HitBox && GetOwner())
    {
        if (USceneComponent* Root = GetOwner()->GetRootComponent())
        {
            HitBox->AttachToComponent(Root, FAttachmentTransformRules::KeepRelativeTransform);
            HitBox->RegisterComponent(); // 念のため明示的に登録
        }

        HitBox->OnComponentBeginOverlap.AddDynamic(this, &UFlammableComponent::OnOverlap);
    }
}

void UFlammableComponent::OnOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
    bool bFromSweep, const FHitResult& SweepResult)
{
    if (!OtherActor)
        return;

    IStateControllable* IS = Cast<IStateControllable>(OtherActor);

    if (!IS)
        return;

    IS->ChangeState("Dead");
}
void UFlammableComponent::Ignite()
{
    if (bIsIgnited) return;
    bIsIgnited = true;
    HitBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    if (FireEffectActor && GetOwner())
    {
        FActorSpawnParameters Params;
        SpawnedFire = GetWorld()->SpawnActor<AActor>(FireEffectActor, GetOwner()->GetActorLocation(), FRotator::ZeroRotator, Params);
        SpawnedFire->AttachToActor(GetOwner(), FAttachmentTransformRules::KeepWorldTransform);
    }
}

void UFlammableComponent::Extinguish()
{
    if (!bIsIgnited) return;
    bIsIgnited = false;

    if (SpawnedFire)
    {
        SpawnedFire->Destroy();
        SpawnedFire = nullptr;
    }

    HitBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}
