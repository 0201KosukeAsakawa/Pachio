#include "Logic/Movement/PlayerMoveLogic.h"
#include "InputAction.h"
#include "GameFramework/Character.h"

FVector UPlayerMoveLogic::Movement(float DeltaTime, AActor* Owner, const FInputActionValue& Value)
{
    if (!GetWorld())
        return FVector(0,0,0);

    FVector2D MoveInput = Value.Get<FVector2D>();

    // デッドゾーン処理（小さい入力は無視）
    const float DeadZone = 0.2f;
    if (MoveInput.Size() < DeadZone)
        return FVector(0, 0, 0);

    ACharacter* character = Cast<ACharacter>(Owner);
    if (!character)
        return FVector(0, 0, 0);

    // カメラ回転に基づく移動方向の取得
    FRotator CamRot = character->GetControlRotation();
    FVector CamForward = FRotationMatrix(CamRot).GetUnitAxis(EAxis::X);
    FVector CamRight = FRotationMatrix(CamRot).GetUnitAxis(EAxis::Y);

    // キャラクターが上下逆の場合、左右ベクトルを反転
    if (FVector::DotProduct(Owner->GetActorUpVector(), FVector::UpVector) < 0.f)
    {
        CamRight *= -1.f;
    }

    // カメラ基準での移動ベクトル（ワールド座標）
    FVector WorldMoveDir = (CamRight * MoveInput.X + CamForward * MoveInput.Y).GetSafeNormal();

    // キャラクターのローカル座標に変換
    FVector LocalMoveDir = Owner->GetActorTransform().InverseTransformVectorNoScale(WorldMoveDir);

    // キャラクターの進行方向（ローカル軸）に変換して最終的な移動ベクトルを算出
    FVector MoveDir =
        Owner->GetActorRightVector() * LocalMoveDir.Y +
        Owner->GetActorForwardVector() * LocalMoveDir.X;

    // 入力ベクトルをキャラクターに適用
    return MoveDir;

}




// 移動速度などの初期化関数（未実装）
void UPlayerMoveLogic::Init(float speed, const FVector)
{
    return;
}
