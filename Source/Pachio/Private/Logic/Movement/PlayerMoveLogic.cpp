#include "Logic/Movement/PlayerMoveLogic.h"
#include "InputAction.h"
#include "GameFramework/Character.h"

FVector UPlayerMoveLogic::Movement(float DeltaTime, AActor* Owner, const FInputActionValue& Value)
{
    if (!Owner)
        return FVector::ZeroVector;

    FVector2D MoveInput = Value.Get<FVector2D>();
    const float DeadZone = 0.2f;
    if (MoveInput.Size() < DeadZone)
        return FVector::ZeroVector;

    // オーナーのローカル方向（前と右）に入力をそのまま乗せる
    FVector Forward = Owner->GetActorForwardVector();
    FVector Right = Owner->GetActorRightVector();

    FVector MoveDir = Forward * MoveInput.X + Right * MoveInput.Y;

    // Normalize しなくてもいい場合はここは不要（速度スケールによる）
    MoveDir = MoveDir.GetClampedToMaxSize(1.0f);

    return MoveDir;
}


// 移動速度などの初期化関数（未実装）
void UPlayerMoveLogic::Init(float speed, const FVector)
{
    return;
}
