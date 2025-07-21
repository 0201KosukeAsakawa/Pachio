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
    // Use the owner's Up vector for vertical movement
    FVector UpVector = Owner->GetActorUpVector();

    // MoveInput.X (left/right input) now controls vertical movement (UpVector)
    FVector MoveDir = UpVector * MoveInput.Y;

    // Normalize しなくてもいい場合はここは不要（速度スケールによる）
    MoveDir = MoveDir.GetClampedToMaxSize(1.0f);

    return MoveDir;
}
void ULadderMoveLogic::Init(float speed, const FVector)
{
}
