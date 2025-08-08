// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/Color/FlammableComponent.h"
#include "Components/BoxComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Manager/LevelManager.h"
#include "Manager/ColorManager.h"
#include "Interface/StateControllable.h"
#include "Logic/ColorManager/ColorTargetRegistry.h"
#include "FunctionLibrary.h"

UFlammableComponent::UFlammableComponent()
{
    HitBox = CreateDefaultSubobject<UBoxComponent>(TEXT("FireBox"));
    HitBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    HitBox->SetMobility(EComponentMobility::Movable);
}
void UFlammableComponent::BeginPlay()
{
    Super::BeginPlay();

    AActor* Owner = GetOwner();
    if (!Owner)
        return;


    HitBox = UFunctionLibrary::FindComponentByName<UBoxComponent>(GetOwner(), TEXT("FireBox"));
    if (HitBox)
        HitBox->OnComponentBeginOverlap.AddDynamic(this, &UFlammableComponent::OnOverlap);


    // 🔥 エフェクトのセットアップ
    if (FlameSystem && !FlameEffect)
    {
        FlameEffect = UNiagaraFunctionLibrary::SpawnSystemAttached(FlameSystem, Owner->GetRootComponent(), NAME_None, FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::KeepRelativeOffset, false);
        FlameEffect->SetWorldScale3D(FVector(1.f));
        FlameEffect->Deactivate();
    }

    // 🎨 色のリアクション設定
    ALevelManager::GetInstance(GetWorld())->GetColorManager()->GetColorTargetRegistry()->OnColorApplied.AddDynamic(this, &UFlammableComponent::ColorAction);
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

void UFlammableComponent::ColorAction(EColorTargetType Mode, FLinearColor NewColor)
{
    if (Mode != EColorTargetType::WorldColor)
        return;
    ALevelManager* level = ALevelManager::GetInstance(GetWorld());
    if (level == nullptr)
        return;

    UColorManager* colorManager = level->GetColorManager();
    if (colorManager == nullptr)
        return;

    FEffectMatchResult Match = ALevelManager::GetInstance(GetWorld())
        ->GetColorManager()
        ->GetClosestEffectByHue(NewColor);

    if (!balwaysBurning)
    {
        if (Match.ClosestEffect == EBuffEffect::Red)
        {
            Ignite();
        }
        else  if (Match.ClosestEffect == EBuffEffect::Blue)
        {
            Extinguish();
        }
    }
    else
    {
        if (Match.ClosestEffect == EBuffEffect::Blue)
        {
            Extinguish();
        }
        else 
        {
            Ignite();
        }
    }
  
}
