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

void ALowGravityZone::ColorAction(const FLinearColor InColor, FEffectMatchResult result)
{
    if (!ColorConfigurator || !ZoneBox)
        return;
    AColorReactiveObject::ColorAction(InColor,result);
    // �F����v���Ȃ� �� �S���߂�
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
        b = false;
        return;
    }
    if (UniverseEffect)
        UniverseEffect->Activate();
    b = true;
    FVector Center = ZoneBox->GetComponentLocation();
    FVector HalfSize = ZoneBox->GetScaledBoxExtent();

    TArray<FHitResult> HitResults;

    bool bHit = GetWorld()->SweepMultiByChannel(
        HitResults,
        Center,
        Center, // �J�n�E�I�������ŐÓI����
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
                PhysicsComp->SetGravityScale(true, GravityScale,JumpBuff); // ����
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

    if (!b)
        return;
    if (OtherActor && OtherActor != this)
    {
        if (UPhysicsCalculator* PhysicsComp = OtherActor->FindComponentByClass<UPhysicsCalculator>())
        {
            // �ŏ��ɏd�͌y��
            PhysicsComp->SetGravityScale(true, GravityScale, JumpBuff);
            OverlappingActors.Add(OtherActor);
        }
    }
}

void ALowGravityZone::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    if (!b)
        return;
    if (OtherActor && OtherActor != this)
    {
        if (UPhysicsCalculator* PhysicsComp = OtherActor->FindComponentByClass<UPhysicsCalculator>())
        {
            // �d�͂���ɖ߂�
            PhysicsComp->SetGravityScale(true);
            OverlappingActors.Remove(OtherActor);
        }
    }
}