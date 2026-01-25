// Fill out your copyright notice in the Description page of Project Settings.


#include "Logic/Movement/LadderMoveLogic.h"

FVector ULadderMoveLogic::Movement(float DeltaTime, AActor* Owner, const FInputActionValue& Value)
{
    if (!Owner)
        return FVector::ZeroVector;

    FVector2D MoveInput = Value.Get<FVector2D>();
    const float DeadZone = 0.2f;
    if (MoveInput.Size() < DeadZone)
        return FVector::ZeroVector;

    // オーナーのローカル上方向に入力を乗せる
    FVector UpVector = Owner->GetActorUpVector();

    FVector MoveDir = UpVector * MoveInput.X;

    MoveDir = MoveDir.GetClampedToMaxSize(1.0f);

    return MoveDir;
}
void ULadderMoveLogic::Init(const float Speed , const FVector& Direction )
{
}
