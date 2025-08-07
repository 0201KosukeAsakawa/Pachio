// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/Color/LowGravityZone.h"
#include "Components/PhysicsCalculator.h"
#include "Components/BoxComponent.h"
#include "Components/Color/ColorConfigurator.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"

// Sets default values
ALowGravityZone::ALowGravityZone()
{
    PrimaryActorTick.bCanEverTick = false;

    ZoneBox = CreateDefaultSubobject<UBoxComponent>(TEXT("ZoneBox"));
    RootComponent = ZoneBox;

    ZoneBox->SetGenerateOverlapEvents(true);
    ZoneBox->SetCollisionProfileName(TEXT("Trigger"));
}
void ALowGravityZone::Init()
{
    ZoneBox->OnComponentBeginOverlap.AddDynamic(this, &ALowGravityZone::OnOverlapBegin);
    ZoneBox->OnComponentEndOverlap.AddDynamic(this, &ALowGravityZone::OnOverlapEnd);
    if (UniverseSystem && !UniverseEffect)
    {
        UniverseEffect = UNiagaraFunctionLibrary::SpawnSystemAttached(UniverseSystem, GetRootComponent(), NAME_None, FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::KeepRelativeOffset, false);
  /*      UniverseEffect->SetWorldScale3D(FVector(20.f));*/
        UniverseEffect->Deactivate();
    }
}

void ALowGravityZone::ColorAction(const FLinearColor InColor)
{
    if (!ColorConfigurator || !ZoneBox)
        return;

    // 色が一致しない → 全員戻す
    if (!ColorConfigurator->IsColorMatch(InColor))
    {
        for (AActor* Actor : OverlappingActors)
        {
            if (UPhysicsCalculator* PhysicsComp = Actor->FindComponentByClass<UPhysicsCalculator>())
            {
                PhysicsComp->SetGravityScale(true);
            }
        }
        OverlappingActors.Empty();
        if(UniverseEffect)
        UniverseEffect->Deactivate();
        return;
    }
    if (UniverseEffect)
        UniverseEffect->Activate();
    FVector Center = ZoneBox->GetComponentLocation();
    FVector HalfSize = ZoneBox->GetScaledBoxExtent();

    TArray<FHitResult> HitResults;

    bool bHit = GetWorld()->SweepMultiByChannel(
        HitResults,
        Center,
        Center, // 開始・終了同じで静的判定
        FQuat::Identity,
        ECC_PhysicsBody,
        FCollisionShape::MakeBox(HalfSize)
    );

    if (!bHit)
        return;

    for (const FHitResult& Hit : HitResults)
    {
        if (AActor* HitActor = Hit.GetActor())
        {
            if (UPhysicsCalculator* PhysicsComp = HitActor->FindComponentByClass<UPhysicsCalculator>())
            {
                PhysicsComp->SetGravityScale(true, 0.5f); // 半減
                OverlappingActors.Add(HitActor);
            }
        }
    }
}


void ALowGravityZone::SetPostProcessEffectEnabled(bool bEnable)
{
}

void ALowGravityZone::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
    bool bFromSweep, const FHitResult& SweepResult)
{
    if (OtherActor && OtherActor != this)
    {
        if (UPhysicsCalculator* PhysicsComp = OtherActor->FindComponentByClass<UPhysicsCalculator>())
        {
            // 最初に重力軽減
            PhysicsComp->SetGravityScale(true, 0.5f);
            OverlappingActors.Add(OtherActor);
        }
    }
}

void ALowGravityZone::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    if (OtherActor && OtherActor != this)
    {
        if (UPhysicsCalculator* PhysicsComp = OtherActor->FindComponentByClass<UPhysicsCalculator>())
        {
            // 重力を元に戻す
            PhysicsComp->SetGravityScale(true);
            OverlappingActors.Remove(OtherActor);
        }
    }
}