// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/SuperMushroom.h"
#include "Components/SphereComponent.h"
#include "Components/PhysicsCalculator.h"
#include "Components/MoveComponent.h"
#include "FunctionLibrary.h"
#include "Particles/ParticleSystemComponent.h"
#include "Player/PlayerCharacter.h"
#include "Objects/ItemBase.h"


USuperMushroomComponent::USuperMushroomComponent()
{
}

void USuperMushroomComponent::Init(AItemBase* owner)
{
    if (!owner)
        return;
    mOwner = owner;
    moveComp = NewObject<UMoveComponent>(this);
    moveComp->Init(mOwner);
    mOwner->GetPhysics()->AddForce(FVector(0, 1, 1), 1.0f);
}

void USuperMushroomComponent::Update(float DeltaTime)
{
    if (!moveComp)
        return;

    moveComp->Movement(DeltaTime);
   mOwner->GetPhysics()->AddGravity();
}

void USuperMushroomComponent::OnCollected(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
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

        //// アイテムを消す
        //Destroy();
    }
}

void USuperMushroomComponent::SetDirection(FVector direction)
{
    if (!moveComp)
        return;

    moveComp->SetDirection(direction);
}

