// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/Color/FlammableComponent.h"
#include "Components/BoxComponent.h"

UFlammableComponent::UFlammableComponent()
{
    HitBox = CreateDefaultSubobject<UBoxComponent>(TEXT("FireBox"));
}

void UFlammableComponent::BeginPlay()
{
    Super::BeginPlay(); // これがないと不具合が出やすい

    if (HitBox)
    {
        HitBox->OnComponentBeginOverlap.AddDynamic(this, &UFlammableComponent::OnOverlap);
        HitBox->SetupAttachment(GetOwner()->GetRootComponent()); // コンポーネントにアタッチ
    }
}

void UFlammableComponent::OnOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
    bool bFromSweep, const FHitResult& SweepResult)
{

}
void UFlammableComponent::Ignite()
{
    if (bIsIgnited) return;
    bIsIgnited = true;

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
