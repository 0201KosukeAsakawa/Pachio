// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/Fireflower.h"
#include "Components/SphereComponent.h"
#include "Components/PhysicsCalculator.h"
#include "Components/MoveComponent.h"
#include "FunctionLibrary.h"
#include "Particles/ParticleSystemComponent.h"
#include "Interface/StateControllable.h"
#include "Objects/ItemBase.h"

void UFireflower::OnCollected(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (!mOwner)
        return;

    // 他のアクターがマリオかどうかを確認
    if (IStateControllable* Mario = Cast<IStateControllable>(OtherActor))
    {
        // マリオにパワーアップの通知を送る（現在はコメントアウトされている）
        Mario->ChangeState("Fire");

        // パワーアップエフェクトが設定されていれば表示
        if (PowerUpEffect)
        {
            PowerUpEffect->Activate();
        }

        // アイテムを消す処理（コメントアウトされている）
        mOwner->Destroy();
    }
}

void UFireflower::Init(AItemBase* owner)
{
    // アイテムの所有者が有効か確認
    if (!owner)
        return;

    // 所有者の設定
    mOwner = owner;
}

void UFireflower::Update(float DeltaTime)
{
    // 物理計算コンポーネントを使用して重力を加える
    mOwner->GetPhysics()->AddGravity();
}
