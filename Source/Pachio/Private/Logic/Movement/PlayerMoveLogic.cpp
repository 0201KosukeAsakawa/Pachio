#include "Logic/Movement/PlayerMoveLogic.h"
#include "InputAction.h"
#include "GameFramework/Character.h"

FVector UPlayerMoveLogic::Movement(float DeltaTime, AActor* Owner, const FInputActionValue& Value)
{
    if (!Owner) return FVector::ZeroVector;

    FVector2D MoveInput = Value.Get<FVector2D>();
    const float DeadZone = 0.2f;
    if (FMath::Abs(MoveInput.Y) < DeadZone)  // 左右だけを考慮
        return FVector::ZeroVector;

    // Y軸（RightVector）に沿って移動
    FVector MoveDir = Owner->GetActorRightVector() * MoveInput.Y;

    return MoveDir;
}



// 移動速度などの初期化関数（未実装）
void UPlayerMoveLogic::Init(const float Speed, const FVector& Direction)
{
    return;
}
