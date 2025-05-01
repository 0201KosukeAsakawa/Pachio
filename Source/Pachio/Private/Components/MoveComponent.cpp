// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/MoveComponent.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "Components/PrimitiveComponent.h"
#include "GameFramework/PlayerController.h"
#include "Math/UnrealMathUtility.h"

// Sets default values for this component's properties
UMoveComponent::UMoveComponent()
{
    Speed = 600.0f; // 速度の設定

    // 初期方向を設定（ランダムな方向で初期化）
    CurrentMovementDirection = FVector(FMath::RandRange(-1.f, 1.f), FMath::RandRange(-1.f, 1.f), 0.f);
    CurrentMovementDirection.Normalize(); // 方向ベクトルを正規化して長さを1にする
}

void UMoveComponent::Init(AActor* owner)
{
    if (!owner)
        return;
    mOwner = owner;
}

void UMoveComponent::Movement(float DeltaTime)
{
    if (!mOwner)
        return;

    // 衝突判定
    if (IsCollidingWithWall(CurrentMovementDirection))
    {
        // 衝突した場合、移動方向を反転させる
        CurrentMovementDirection = -CurrentMovementDirection;
    }

    // 移動処理
    FVector NewLocation = mOwner->GetActorLocation() + (CurrentMovementDirection * Speed * DeltaTime);
    mOwner->SetActorLocation(NewLocation);
}

void UMoveComponent::SetDirection(FVector NewDirection)
{
    // Directionの設定
    CurrentMovementDirection = NewDirection;
    CurrentMovementDirection.Normalize();  // 新しい方向を正規化
}

bool UMoveComponent::IsCollidingWithWall(FVector Direction)
{
    if (!mOwner)
        return true;

    FVector Start = mOwner->GetActorLocation();
    FVector End = Start + (Direction * Speed * 0.1f);  // 少しだけ前進して衝突をチェック

    FHitResult HitResult;
    FCollisionQueryParams CollisionParams;
    CollisionParams.AddIgnoredActor(mOwner);  // 自身を無視して衝突判定を行う

    // レイキャストで衝突判定を行う
    bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, CollisionParams);

    return bHit;
}
