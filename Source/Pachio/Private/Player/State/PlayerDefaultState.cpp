// プロジェクト設定の Description ページに著作権情報を記入

#include "Player/State/PlayerDefaultState.h"
#include "Player/State/PlayerHoldState.h"
#include "Player/PlayerCharacter.h"
#include "Player/InGameController.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/MoveComponent.h"
#include "Components/BoxComponent.h"
#include "Components/PhysicsCalculator.h"
#include "FunctionLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "InputActionValue.h"
#include "Interface/StateControllable.h"
#include "Interface/Soundable.h"
#include "Kismet/KismetMathLibrary.h"
#include "Logic/Movement/PlayerMoveLogic.h"
#include "Manager/LevelManager.h"
#include "UI/UIManager.h"
#include "Objects/ColorProjectile.h"
#include "Components/Color/ColorControllerComponent.h"
#include "ColorUtilityLibrary.h"
#include "Components/Color/ObjectColorComponent.h"
namespace Player_DEFAULT_Constants
{
    constexpr float DEAD_ZONE = 0.2f;
    constexpr float AIR_INPUT_SCALE = 0.3f;
    constexpr float GROUND_INPUT_SCALE = 1.0f;
    constexpr float SKILL_RANGE = 200.0f;
    constexpr float JUMP_START_IGNORE_DURATION = 0.1f;
    constexpr float AIR_CONTROL = 0.2f;
    constexpr float FALLING_LATERAL_FRICTION = 0.5f;


}

UPlayerDefaultState::UPlayerDefaultState()
    :LaunchAngle(30.f)
    , LaunchSpeed(1000.f)
{
}

// ステートに入る際に実行される処理
bool UPlayerDefaultState::OnEnter(APawn* owner)
{
    // 所有キャラクターまたはワールドが無効な場合は失敗
    if (owner == nullptr)
    {
        return false;
    }

    // 内部に所有者とワールドを保存
        mOwner = owner;
    if (!MoveComp)
    {
        MoveComp = NewObject<UMoveComponent>(mOwner);
        UPlayerMoveLogic* PlayerLogic = NewObject<UPlayerMoveLogic>(this);
        MoveComp->Init(PlayerLogic);
    }

    if (!Physics)
    {
        Physics = GetOwner()->GetComponentByClass<UPhysicsCalculator>();
    }


    if (!HitBox)
    {
        HitBox = GetOwner()->GetComponentByClass<UCapsuleComponent>();
    }


    // キャラクターが持つ StaticMeshComponent を取得
    UStaticMeshComponent* StaticMeshComp = UFunctionLibrary::FindComponentByName<UStaticMeshComponent>(owner, "StaticMesh");
    UMaterialInterface* N = NewMaterial.LoadSynchronous(); // 非同期ロードに対応
    if (N != nullptr && StaticMeshComp)
    {
        StaticMeshComp->SetMaterial(0, N); // マテリアルをスロット0に適用
    }

    APlayerCharacter* aPlayer = Cast<APlayerCharacter>(mOwner);
    if (!aPlayer)
        return false;
    APlayerController* PC = GetWorld()->GetFirstPlayerController();
    if (PC)
    {
        PC->SetInputMode(FInputModeGameOnly());
        PC->bShowMouseCursor = false;  // 必要に応じてマウスカーソルを非表示に
    }
    // CharacterMovementComponentの設定（初期化時など）
    UCharacterMovementComponent* CharMovement = Cast<UCharacterMovementComponent>(mOwner->GetMovementComponent());
    if (CharMovement)
    {
        // 空中制御の強さ（0.0～1.0、デフォルトは0.05程度）
        CharMovement->AirControl = 0.2f;

        // 空中での減速率（0.0～1.0）
        CharMovement->FallingLateralFriction = 0.5f;
    }
    // 移動速度の初期値設定（ステート内で使用）
    mMoveSpeed;
    CurrentDirection = mOwner->GetActorForwardVector();

    return true; // ステートの切り替え成功
}

// ステートの毎フレーム更新処理（現時点では何もしない）
bool UPlayerDefaultState::OnUpdate(float DeltaTime)
{
    if (GetWorld() == nullptr || Physics == nullptr)
        return false;

    UCharacterMovementComponent* CharMovement = Cast<UCharacterMovementComponent>(mOwner->GetMovementComponent());

    if (bIsJumping && CharMovement)
    {
        bool bOnGround = Physics->OnGround();
        float CurrentTime = GetWorld()->GetTimeSeconds();
        float TimeSinceJump = CurrentTime - JumpStartTime;

        // ジャンプ開始から一定時間経過後に着地した場合
        if (bOnGround && TimeSinceJump > JumpStartIgnoreDuration)
        {
            // 力を中止
            Physics->AddForce(FVector(0, 0, 0), 0.f);

            // 移動モードをWalkingに戻す
            CharMovement->SetMovementMode(MOVE_Walking);

            ISoundManagerProvider* sound = ALevelManager::GetInstance(GetWorld())->GetSoundManager().GetInterface();
            if (sound)
            {
                UE_LOG(LogTemp, Warning, TEXT("Landed after jump!"));
                sound->PlaySound(ESoundKinds::SE, "Land");
            }

            bIsJumping = false;
        }
    }
    else if (Physics->HasLanded() && CharMovement)
    {
        // 通常の着地処理（ジャンプ以外で落下した場合）
        CharMovement->SetMovementMode(MOVE_Walking);

        ISoundManagerProvider* sound = ALevelManager::GetInstance(GetWorld())->GetSoundManager().GetInterface();
        if (sound)
        {
            UE_LOG(LogTemp, Warning, TEXT("HasLanded returned true, entering if-block!"));
            sound->PlaySound(ESoundKinds::SE, "Land");
        }
    }

    return true;
}
bool UPlayerDefaultState::OnExit(APawn* owner)
{
    return true;
}
bool UPlayerDefaultState::OnSkill(const FInputActionValue& Value)
{
    ACharacter* Character = Cast<ACharacter>(GetOwner());
    if (!Character) return false;
    UColorControllerComponent* ColorComp = GetOwner()->GetComponentByClass<UColorControllerComponent>();
    if (!ColorComp) return false;
    // モード判定
    if (mode == EColorAbsorbMode::Paint)
    {
        CurrentSelectedColor = ColorComp->GetCurrentColor();
        EColorCategory Category =
            UColorUtilityLibrary::GetNearestColorCategoryRGBY(CurrentSelectedColor);

        int32* TankValue = ColorComp->ColorTankMap.Find(Category);
        if (!TankValue || *TankValue <= 0)
        {
            UE_LOG(LogTemp, Warning, TEXT("Not enough color in tank"));
            return false;
        }

        // 吸うモード → 近くのオブジェクトを吸収
        UObjectColorComponent* TargetComp = ColorComp->GetHitColorComponent(500.f); // 距離100を例
        if (TargetComp)
        {
            TargetComp->SetTargetColor(CurrentSelectedColor);
            (*TankValue)--; // 成功時のみ消費
            UE_LOG(LogTemp, Log, TEXT("Paint Hit! Tank remaining: %d"), *TankValue);
        }
      }
    else if (mode == EColorAbsorbMode::Absorb)
    {
        // 吸うモード → 近くのオブジェクトを吸収
        UObjectColorComponent* TargetComp = ColorComp->GetHitColorComponent(500.f); // 距離100を例
        if (TargetComp)
        {
            ColorComp->AbsorbHitObject(TargetComp);
        }
    }
    return true;
}

void UPlayerDefaultState::Movement(const FInputActionValue& Value)
{
    const FVector2D MoveInput = Value.Get<FVector2D>();
    constexpr float DeadZone = 0.2f;

    UCharacterMovementComponent* CharMovement =
        Cast<UCharacterMovementComponent>(mOwner->GetMovementComponent());

    // ★ ワールド軸直指定
    FVector Direction(
        MoveInput.X, // W / S → X
        MoveInput.Y, // D / A → Y
        0.f
    );

    if (!Direction.Normalize())
    {
        return;
    }

    // アニメーション用（XY）
    MoveDelta = Direction * 100.f * GetWorld()->GetDeltaSeconds();

    const bool bIsInAir = CharMovement && CharMovement->IsFalling();
    const float InputScale = bIsInAir ? 0.3f : 1.0f;

    mOwner->AddMovementInput(Direction, InputScale);
}




bool UPlayerDefaultState::Jump(float jumpForce)
{
    if (GetOwner() == nullptr || Physics == nullptr || !Physics->OnGround())
    {
        return false;
    }

    // ジャンプ力を掛けて力を加える
    Physics->AddForce(GetOwner()->GetActorUpVector(), jumpForce);

    // 移動モードをFallingに切り替え
    UCharacterMovementComponent* CharMovement = Cast<UCharacterMovementComponent>(mOwner->GetMovementComponent());
    if (CharMovement)
    {
        CharMovement->SetMovementMode(MOVE_Falling);
    }

    // ジャンプ開始情報を記録
    bIsJumping = true;
    JumpStartTime = GetWorld()->GetTimeSeconds();

    ISoundManagerProvider* sound = ALevelManager::GetInstance(GetWorld())->GetSoundManager().GetInterface();
    if (sound)
        sound->PlaySound(ESoundKinds::SE, "Jump");
    return true;
}
