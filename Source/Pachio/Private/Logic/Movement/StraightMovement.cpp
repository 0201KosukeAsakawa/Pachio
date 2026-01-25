// Fill out your copyright notice in the Description page of Project Settings.


#include "Logic/Movement/StraightMovement.h"

// コンストラクタ：特に初期化処理はなし
UStraightMovement::UStraightMovement()
{
}

// 敵キャラクターの移動処理（毎フレーム呼ばれる）
// DeltaTime: 経過時間, Owner: 操作対象アクター, direction: 移動方向ベクトル（外部指定）
FVector UStraightMovement::Movement(float DeltaTime, AActor* Owner,const FInputActionValue& Value)
{
    if (!Owner)
        return FVector::ZeroVector;

    FVector moveDir = CurrentMovementDirection;

    // 壁に接触している場合は方向反転
    if (IsCollidingWithWall(moveDir, Owner))
    {
        moveDir.Y *= -1.0f;

        // 進行方向に向くようにアクターの回転を更新
        FRotator NewRotation = moveDir.ToOrientationRotator();
        Owner->SetActorRotation(NewRotation);
    }

    // 移動先の位置を返す
    return Owner->GetActorLocation() + (moveDir * Speed * DeltaTime);
}


// 壁との接触をチェックする関数
// 指定された方向に壁があるかをレイキャストで判定する
bool UStraightMovement::IsCollidingWithWall(FVector Direction, const AActor* Owner)
{
    if (!Owner)
        return true;  // 所有者が無効なら接触しているとみなす

    FVector Start = Owner->GetActorLocation();  // レイの始点：現在位置
    FVector End = Start + (Direction * Speed * 0.0001f);  // レイの終点：ほんの少し先

    FHitResult HitResult;
    FCollisionQueryParams CollisionParams;
    CollisionParams.AddIgnoredActor(Owner);  // 自分自身との衝突は無視

    // 可視チャネル（ECC_Visibility）を使ってレイキャスト
    bool bHit = Owner->GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, CollisionParams);

    // プレイヤーキャラなど "ACharacter" に衝突した場合は無視（たとえば踏みつけ可能）
    if (Cast<ACharacter>(HitResult.GetActor()))
        return false;

    return bHit;  // それ以外の何かに当たっていれば「壁に接触」とみなす
}

// 初期化処理（移動速度と初期方向を設定）
void UStraightMovement::Init(const float speed, const FVector& Direction)
{    
    Speed = speed;
    CurrentMovementDirection = Direction;
}
