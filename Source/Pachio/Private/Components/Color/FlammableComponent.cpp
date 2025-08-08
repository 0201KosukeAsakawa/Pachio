// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/Color/FlammableComponent.h"
#include "Components/BoxComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Interface/StateControllable.h"

UFlammableComponent::UFlammableComponent()
{
    HitBox = CreateDefaultSubobject<UBoxComponent>(TEXT("FireBox"));
    HitBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    HitBox->SetMobility(EComponentMobility::Movable);
}
void UFlammableComponent::BeginPlay()
{
    Super::BeginPlay();
    if (FlameSystem && !FlameEffect)
    {
        FlameEffect = UNiagaraFunctionLibrary::SpawnSystemAttached(FlameSystem, GetOwner()->GetRootComponent(), NAME_None, FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::KeepRelativeOffset, false);
        FlameEffect->SetWorldScale3D(FVector(1.f));
        FlameEffect->Deactivate();
    }
    if (HitBox && GetOwner())
    {
        if (USceneComponent* Root = GetOwner()->GetRootComponent())
        {
            HitBox->AttachToComponent(Root, FAttachmentTransformRules::KeepRelativeTransform);
            HitBox->OnComponentBeginOverlap.AddDynamic(this, &UFlammableComponent::OnOverlap);
        }
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
    if(FlameEffect)
    FlameEffect->Activate();
}

void UFlammableComponent::Extinguish()
{
    if (!bIsIgnited) return;
    bIsIgnited = false;

    if (FlameEffect) FlameEffect->Deactivate();

    HitBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}
