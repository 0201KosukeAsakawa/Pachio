// Fill out your copyright notice in the Description page of Project Settings.


#include "Logic/Movement/EnemyMoveLogic.h"

UEnemyMoveLogic::UEnemyMoveLogic()
{

}


FVector UEnemyMoveLogic::Movement(float DeltaTime, AActor* Owner, const FInputActionValue& Value)
{
    if (!Owner)
        return FVector(0,0,0);

    // 衝突判定
    if (IsCollidingWithWall(CurrentMovementDirection,Owner))
    {
        // Y軸方向に反転させる
        CurrentMovementDirection.Y = -CurrentMovementDirection.Y;

        // 反転後の方向に基づいて、移動するためのActorの回転を設定
        FRotator NewRotation = CurrentMovementDirection.ToOrientationRotator();
        Owner->SetActorRotation(NewRotation); // 新しい回転を設定
    }

    // 移動処理
    return Owner->GetActorLocation() + (CurrentMovementDirection * Speed * DeltaTime);
}

bool UEnemyMoveLogic::IsCollidingWithWall(FVector Direction,const AActor* Owner)
{
    if (!Owner)
        return true;

    FVector Start = Owner->GetActorLocation();
    FVector End = Start + (Direction * Speed * 0.0001f);  // 少しだけ前進して衝突をチェック

    FHitResult HitResult;
    FCollisionQueryParams CollisionParams;
    CollisionParams.AddIgnoredActor(Owner);  // 自身を無視して衝突判定を行う

    // レイキャストで衝突判定を行う
    bool bHit = Owner->GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, CollisionParams);
    if (Cast<ACharacter>(HitResult.GetActor()))
        return false;
    return bHit;
}