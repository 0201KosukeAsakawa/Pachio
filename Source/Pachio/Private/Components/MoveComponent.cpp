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
}

void UMoveComponent::Init(TScriptInterface<IMoveLogic>moveLogic, const float speed, const FVector NewDirection)
{
    Speed = speed;
    MoveLogic = moveLogic;
    MoveLogic->Init(speed,NewDirection);
}

FVector UMoveComponent::Movement(float DeltaTime, AActor* Owner, const FInputActionValue& Value)
{
    if (!MoveLogic)
    {
        return FVector(0, 0, 0);
    }
        return MoveLogic->Movement(DeltaTime,Owner, Value);
}
void UMoveComponent::SetDirection(FVector NewDirection)
{
    MoveLogic->Init(Speed, NewDirection);
}

bool UMoveComponent::SetMoveLogic(TScriptInterface<IMoveLogic>Logic)
{
    if(!Logic)
    return false;

    MoveLogic = Logic;
    return true;
}
