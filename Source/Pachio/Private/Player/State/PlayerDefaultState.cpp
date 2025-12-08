// プロジェクト設定の Description ページに著作権情報を記入

#include "Player/State/PlayerDefaultState.h"
#include "Player/State/LadderClimberState.h"
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
#include "Objects/Color/LadderActor.h"

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

            ISoundable* sound = ALevelManager::GetInstance(GetWorld())->GetSoundManager().GetInterface();
            if (sound)
            {
                UE_LOG(LogTemp, Warning, TEXT("Landed after jump!"));
                sound->PlaySound("SE", "Land");
            }

            bIsJumping = false;
        }
    }
    else if (Physics->HasLanded() && CharMovement)
    {
        // 通常の着地処理（ジャンプ以外で落下した場合）
        CharMovement->SetMovementMode(MOVE_Walking);

        ISoundable* sound = ALevelManager::GetInstance(GetWorld())->GetSoundManager().GetInterface();
        if (sound)
        {
            UE_LOG(LogTemp, Warning, TEXT("HasLanded returned true, entering if-block!"));
            sound->PlaySound("SE", "Land");
        }
    }

    return true;
}
// ステートを離脱するときの処理（現時点では何もしない）
bool UPlayerDefaultState::OnExit(APawn*)
{
    return true;
}

// スキルボタン入力時の処理（現時点では何もしない）
bool UPlayerDefaultState::OnSkill(const FInputActionValue& Value)
{
    if (!Value.Get<bool>())
        return false;

    // 目の前に持てるオブジェクトがあるか判定
    FVector Start = mOwner->GetActorLocation();
    FVector End = Start + CurrentDirection * 200.f; // 2m先まで

    FHitResult Hit;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(mOwner);
    bool bIsHit = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params);
    if (!bIsHit)
        return false;

    AActor* Target = Hit.GetActor();
    if (Target == nullptr || !Target->ActorHasTag("Holdable")) // 持てるオブジェクトにタグを付けておく
        return false;

    IStateControllable* Player = Cast<IStateControllable>(mOwner);
    if (Player == nullptr)
        return false;
    UPlayerStateComponent* NewState = Player->ChangeState(EPlayerStateType::Hold);
    if (NewState == nullptr)
        return false;

    if (UPlayerHoldState* HoldState = Cast<UPlayerHoldState>(NewState))
    {
        if (CurrentDirection.Y > 0)
            HoldState->SetUp(Target, true);
        else
            HoldState->SetUp(Target, false);
    }

    return true;
}

void UPlayerDefaultState::Movement(const FInputActionValue& Value)
{
    FVector2D MoveInput = Value.Get<FVector2D>();
    float DeadZone = 0.2f;

    UCharacterMovementComponent* CharMovement = Cast<UCharacterMovementComponent>(mOwner->GetMovementComponent());

    if (MoveInput.X >= DeadZone && TryEnterLadderOnJump())
    {
        Physics->AddForce(FVector(0, 0, 0), 0.f);
        Physics->SetGravityScale(false);
        return;
    }

    FVector direction = MoveComp->Movement(0, mOwner, Value);
    direction.Normalize();

    if (direction != FVector::ZeroVector)
    {
        FRotator CurrentRotation = mOwner->GetActorRotation();
        float TargetYaw;

        if (MoveInput.Y > 0)
        {
            TargetYaw = 0.f;
        }
        else if (MoveInput.Y < 0)
        {
            TargetYaw = 180.f;
            direction *= -1;
        }
        else
        {
            TargetYaw = direction.Rotation().Yaw;
        }

        CurrentDirection = FVector(0, direction.Y, 0);

        if (!FMath::IsNearlyEqual(CurrentRotation.Yaw, TargetYaw, 1.f))
        {
            FRotator NewRotation = FRotator(CurrentRotation.Pitch, TargetYaw, CurrentRotation.Roll);
            mOwner->SetActorRotation(NewRotation);
        }
    }

    // MoveDeltaを計算（アニメーション用）
    MoveDelta = direction * 100* GetWorld()->GetDeltaSeconds();
    //UE_LOG(LogTemp, Log, TEXT("MoveDelta: X=%f, Y=%f, Z=%f "),
    //    MoveDelta.X, MoveDelta.Y, MoveDelta.Z);
    // ★空中では入力スケールを小さくする
    bool bIsInAir = CharMovement && CharMovement->IsFalling();
    float InputScale = bIsInAir ? 0.3f : 1.0f;  // 空中では30%の入力

    mOwner->AddMovementInput(direction, InputScale);
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

    ISoundable* sound = ALevelManager::GetInstance(GetWorld())->GetSoundManager().GetInterface();
    sound->PlaySound("SE", "Jump");
    return true;
}

bool UPlayerDefaultState::TryEnterLadderOnJump() const
{
    if (mOwner == nullptr)
        return false;

    // プレイヤーにアタッチされたBoxComponentを用意している想定
    // 例えば LadderCheckTrigger として UBoxComponent* を保持している
    if (HitBox == nullptr)
        return false;

    TArray<AActor*> OverlappingActors;
    HitBox->GetOverlappingActors(OverlappingActors, ALadderActor::StaticClass());

    if (OverlappingActors.Num() == 0)
        return false;

    IStateControllable* player = Cast<IStateControllable>(GetOwner());
    if (player == nullptr)
        return false;

    for (AActor* Actor : OverlappingActors)
    {
        if (!Actor->ActorHasTag("Ladder"))
            continue;

        ALadderActor* Ladder = Cast<ALadderActor>(Actor);
        if (!Ladder)
            continue;

        // ステート切り替え
        if (UPlayerStateComponent* NewState = player->ChangeState(EPlayerStateType::Climb))
        {
            if (ULadderClimberState* ClimbState = Cast<ULadderClimberState>(NewState))
            {
                ClimbState->SetTargetLadder(Ladder);
                return true;
            }
        }
    }

    return false;

}