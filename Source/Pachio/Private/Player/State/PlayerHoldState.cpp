// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/State/PlayerHoldState.h"
#include "Manager/LevelManager.h"
#include "Sound/SoundManager.h"
#include "InputActionValue.h"
#include "Interface/StateControllable.h"
#include "Logic/Movement/PlayerMoveLogic.h"
#include "Components/Color/ColorReactiveComponent.h"
#include "Components/MoveComponent.h"

UPlayerHoldState::UPlayerHoldState()
    : HoldTarget(nullptr)
    , GrabDirection(1)
{
}

bool UPlayerHoldState::OnEnter(APawn* owner)
{
    mOwner = owner;
    if (!MoveComp)
    {
        MoveComp = NewObject<UMoveComponent>(mOwner);
        UPlayerMoveLogic* PlayerLogic = NewObject<UPlayerMoveLogic>(this);
        MoveComp->Init(PlayerLogic);
    }
    return true;
}

bool UPlayerHoldState::OnUpdate(float DeltaTime)
{
    if (HoldTarget == nullptr || mOwner == nullptr || targetComp == nullptr) return false;
    if (targetComp->IsHidden())
    {
        // Hold解除して Default に戻す
        if (IStateControllable* Player = Cast<IStateControllable>(mOwner))
        {
            Player->ChangeState(EPlayerStateType::Default);
        }
        return true;
    }

    // 距離チェック
    float distance = FVector::Dist(mOwner->GetActorLocation(), HoldTarget->GetActorLocation());
    const float MaxDistance =20 ; // 離れすぎる距離

    if (distance > InitialHoldDistance + MaxDistance)
    {
        // Hold解除して Default に戻す
        if (IStateControllable* Player = Cast<IStateControllable>(mOwner))
        {
            Player->ChangeState(EPlayerStateType::Default);
        }
    }
    return true;
}

bool UPlayerHoldState::OnExit(APawn* owner)
{
    HoldTarget = nullptr;
    targetComp = nullptr;
    ALevelManager::GetInstance(GetWorld())->GetSoundManager()->PlaySound("SE", "Put");
    return true;
}


bool UPlayerHoldState::OnSkill(const FInputActionValue& Value)
{
    if (Value.Get<bool>())
    {
        // Default に戻す
        if (IStateControllable* Player = Cast<IStateControllable>(mOwner))
        {
            Player->ChangeState(EPlayerStateType::Default);
        }
    }
    return true;
}
void UPlayerHoldState::Movement(const FInputActionValue& Value)
{
    if (!HoldTarget || !MoveComp) return;

    FVector direction = MoveComp->Movement(0, mOwner, Value);
    if (direction.IsNearlyZero())
    {
        MoveDelta = FVector::ZeroVector;
        return;
    }

    // 移動方向を入力から決定
    if (direction.Y < 0)
    {
        MoveDirection = -1;
    }
    else if (direction.Y > 0)
    {
        MoveDirection = 1;
    }
    else
    {
        MoveDirection = 0;
    }

    // 掴んだ向き (direc) と現在の移動方向 (d) を組み合わせる

    MoveDelta = direction * 700 * GetWorld()->GetDeltaSeconds() * GrabDirection;

    // キャラと箱を同じ移動量で動かす
    mOwner->AddActorWorldOffset(MoveDelta, true);
    HoldTarget->AddActorWorldOffset(MoveDelta, true);
}

void UPlayerHoldState::SetUp(AActor* target, bool b)
{
    HoldTarget = target;

    if (HoldTarget && mOwner)
    {
        InitialHoldDistance = FVector::Dist(mOwner->GetActorLocation(), HoldTarget->GetActorLocation());
        targetComp = HoldTarget->GetComponentByClass<UColorReactiveComponent>();
    }

    // 掴んだ時の向きを固定値として保持
    GrabDirection = b ? 1 : -1;
}