// Fill out your copyright notice in the Description page of Project Settings.

#include "Objects/SuperMushroom.h"
#include "Components/SphereComponent.h"
#include "Components/PhysicsCalculator.h"
#include "Components/MoveComponent.h"
#include "FunctionLibrary.h"
#include "Particles/ParticleSystemComponent.h"
#include "Interface/StateControllable.h"
#include "Objects/ItemBase.h"

USuperMushroomComponent::USuperMushroomComponent()
{
    // コンストラクタ: 特に初期化は行っていない
}

void USuperMushroomComponent::Init(AItemBase* owner)
{
    // アイテムの所有者が有効か確認
    if (!owner)
        return;

    // 所有者の設定
    mOwner = owner;

    // 移動コンポーネントの初期化
    moveComp = NewObject<UMoveComponent>(this);
    moveComp->Init(mOwner, this);  // 移動コンポーネントに所有者を設定
    moveComp->SetSpeed(0.0f);
}

void USuperMushroomComponent::Update(float DeltaTime)
{
    // 移動コンポーネントが有効か確認
    if (!moveComp)
        return;

    // 移動コンポーネントで移動処理を行う
    moveComp->Movement(DeltaTime,mOwner);

    // 物理計算コンポーネントを使用して重力を加える
    mOwner->GetPhysics()->AddGravity();
}

void USuperMushroomComponent::OnCollected(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (!mOwner)
        return;

    // 他のアクターがマリオかどうかを確認
    if (IStateControllable* Mario = Cast<IStateControllable>(OtherActor))
    {
        // マリオにパワーアップの通知を送る（現在はコメントアウトされている）
        Mario->ChangeState("Super");

        // パワーアップエフェクトが設定されていれば表示
        if (PowerUpEffect)
        {
            PowerUpEffect->Activate();
        }

        // アイテムを消す処理（コメントアウトされている）
        mOwner->Destroy();
    }
}

void USuperMushroomComponent::SetDirection(FVector direction)
{
    // 移動コンポーネントが有効か確認
    if (!moveComp)
        return;

    // 移動コンポーネントに方向を設定
    moveComp->SetDirection(direction);
}