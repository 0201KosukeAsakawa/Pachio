// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/MoveComponent.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "Components/PrimitiveComponent.h"
#include "GameFramework/PlayerController.h"
#include "Math/UnrealMathUtility.h"
#include "Interface/MoveLogic.h"

// Sets default values for this component's properties
UMoveComponent::UMoveComponent()
{
    Speed = 600.0f; // 速度の設定

    // 初期方向を設定（ランダムな方向で初期化）
    CurrentMovementDirection = FVector(0.0f, FMath::RandRange(-1.f, 1.f), 0.f);
    CurrentMovementDirection.Normalize(); // 方向ベクトルを正規化して長さを1にする
}

void UMoveComponent::Init(AActor* owner, TScriptInterface<IMoveLogic>moveLogic)
{
    if (!owner)
        return;
    mOwner = owner;

    MoveLogic = moveLogic;
}

FVector UMoveComponent::Movement(float DeltaTime, AActor* Owner, const FInputActionValue& Value)
{
    if (!MoveLogic)
        return FVector(0,0,0);

        return MoveLogic->Movement(DeltaTime,Owner,Value);
}
void UMoveComponent::SetDirection(FVector NewDirection)
{
    // Directionの設定
    CurrentMovementDirection = NewDirection;
    CurrentMovementDirection.Normalize();  // 新しい方向を正規化
}

bool UMoveComponent::SetMoveLogic(TScriptInterface<IMoveLogic>Logic)
{
    if(!Logic)
    return false;

    MoveLogic = Logic;
    return true;
}
