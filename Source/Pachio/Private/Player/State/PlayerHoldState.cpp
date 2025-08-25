// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/State/PlayerHoldState.h"
#include "InputActionValue.h"
#include "Interface/StateControllable.h"
#include "Logic/Movement/PlayerMoveLogic.h"
#include "Components/MoveComponent.h"

UPlayerHoldState::UPlayerHoldState()
    : HoldTarget(nullptr)
{
}

bool UPlayerHoldState::OnEnter(ACharacter* owner, UWorld* world)
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
    if (!HoldTarget || !mOwner) return false;

    if (HoldTarget->IsHidden())
    {
        // Hold解除して Default に戻す
        if (IStateControllable* Player = Cast<IStateControllable>(mOwner))
        {
            Player->ChangeState("Default");
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
            Player->ChangeState("Default");
        }
    }
    return true;
}

bool UPlayerHoldState::OnExit(ACharacter* owner)
{
    HoldTarget = nullptr;
    return true;
}


bool UPlayerHoldState::OnSkill(const FInputActionValue& Value)
{
    if (Value.Get<bool>())
    {
        // Default に戻す
        if (IStateControllable* Player = Cast<IStateControllable>(mOwner))
        {
            Player->ChangeState("Default");
        }
    }
    return true;
}

void UPlayerHoldState::Movement(const FInputActionValue& Value)
{
    if (!HoldTarget || !MoveComp) return;

    FVector direction = MoveComp->Movement(0, mOwner, Value);
    if (direction.IsNearlyZero()) return;

    FVector moveDelta = direction * 700 * GetWorld()->GetDeltaSeconds();

    // キャラと箱を同じ移動量で動かす
    mOwner->AddActorWorldOffset(moveDelta, true);
    HoldTarget->AddActorWorldOffset(moveDelta, true);
}



void UPlayerHoldState::SetUp(AActor* target)
{
    HoldTarget = target;
    if (HoldTarget && mOwner)
    {
        InitialHoldDistance = FVector::Dist(mOwner->GetActorLocation(), HoldTarget->GetActorLocation());
    }
}
