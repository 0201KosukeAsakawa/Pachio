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

UPlayerDefaultState::UPlayerDefaultState()
{
}

// ステートに入る際に実行される処理
bool UPlayerDefaultState::OnEnter(APawn* owner, UWorld* world)
{
    // 所有キャラクターまたはワールドが無効な場合は失敗
    if (owner == nullptr || world == nullptr)
    {
        return false;
    }

    // 内部に所有者とワールドを保存
    if (!mOwner)
        mOwner = owner;
    if (!pWorld)
        pWorld = world;
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


    if (USkeletalMeshComponent* MeshComp = aPlayer->FindComponentByClass<USkeletalMeshComponent>())
    {
        if (UAnimInstance* AnimInstance = MeshComp->GetAnimInstance())
        {
            // モンタージュ終了時のデリゲートをバインド
            AnimInstance->OnMontageEnded.AddDynamic(this, &UPlayerDefaultState::OnLandingMontageEnded);
        }
    }


    bIsPlayingLandingAnimation = false;
    APlayerController* PC = GetWorld()->GetFirstPlayerController();
    if (PC)
    {
        PC->SetInputMode(FInputModeGameOnly());
        PC->bShowMouseCursor = false;  // 必要に応じてマウスカーソルを非表示に
    }

    // 移動速度の初期値設定（ステート内で使用）
    mMoveSpeed = 100.0f;
    CurrentDirection = mOwner->GetActorForwardVector();

    return true; // ステートの切り替え成功
}

// ステートの毎フレーム更新処理（現時点では何もしない）
bool UPlayerDefaultState::OnUpdate(float DeltaTime)
{
    if (GetWorld() == nullptr || Physics == nullptr)
        return false;

    // 前フレームの終了フラグをリセット
    bLandingAnimationJustEnded = false;

    // 着地判定
    if (Physics->HasLanded() && !bIsPlayingLandingAnimation)
    {
        ISoundable* sound = ALevelManager::GetInstance(GetWorld())->GetSoundManager().GetInterface();
        if (sound)
        {
            UE_LOG(LogTemp, Warning, TEXT("HasLanded returned true, playing landing sound and animation!"));
            sound->PlaySound("SE", "Land");
        }

        PlayLandingAnimation();
    }

    if (bIsPlayingLandingAnimation)
    {
        MoveDelta = FVector{ 0,0,0 };
    }


    return true;
}



bool UPlayerDefaultState::OnExit(APawn* owner)
{
    // アニメーション再生中の場合は停止
    if (bIsPlayingLandingAnimation && LandingMontage != nullptr)
    {
        APlayerCharacter* aPlayer = Cast<APlayerCharacter>(mOwner);
        if (aPlayer)
        {
            if (USkeletalMeshComponent* MeshComp = aPlayer->FindComponentByClass<USkeletalMeshComponent>())
            {
                if (UAnimInstance* AnimInstance = MeshComp->GetAnimInstance())
                {
                    AnimInstance->Montage_Stop(0.2f, LandingMontage);
                }
            }
        }

        // 入力を再有効化（念のため）
        APlayerController* PC = GetWorld()->GetFirstPlayerController();
        if (PC)
        {
            mOwner->EnableInput(PC);
        }

        bIsPlayingLandingAnimation = false;
    }

    return true;
}

// スキルボタン入力時の処理（現時点では何もしない）
bool UPlayerDefaultState::OnSkill(const FInputActionValue& Value)
{
    if (!Value.Get<bool>())
        return false;

    if (bIsPlayingLandingAnimation)
    {
        return false; // または return;
    }

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
    // アニメーション再生中は移動を無効化
    if (bIsPlayingLandingAnimation)
    {
        return;
    }

    FVector2D MoveInput = Value.Get<FVector2D>();

    float DeadZone = 0.2f;
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

    MoveDelta = direction * MoveSpeed * GetWorld()->GetDeltaSeconds();
    mOwner->AddMovementInput(direction, MoveSpeed);
}




bool UPlayerDefaultState::Jump(float jumpForce)
{
    // アニメーション再生中はジャンプを無効化
    if (bIsPlayingLandingAnimation)
    {
        return false;
    }

    if (GetOwner() == nullptr || Physics == nullptr || !Physics->OnGround())
        return false;

    Physics->AddForce(GetOwner()->GetActorUpVector(), jumpForce);
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

void UPlayerDefaultState::OnLandingMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
    if (Montage == LandingMontage)
    {
        bIsPlayingLandingAnimation = false;
        bLandingAnimationJustEnded = true; // OnUpdateで自動的にfalseになる

        APlayerController* PC = GetWorld()->GetFirstPlayerController();
        if (PC && mOwner)
        {
            // 入力を再有効化
            mOwner->EnableInput(PC);

            // 物理的な速度をリセット
            APlayerCharacter* aPlayer = Cast<APlayerCharacter>(mOwner);
            if (aPlayer)
            {
                if (UCharacterMovementComponent* MovementComp = aPlayer->GetCharacterMovement())
                {
                    MovementComp->Velocity = FVector::ZeroVector;
                    MovementComp->StopMovementImmediately();
                }
                aPlayer->ConsumeMovementInputVector();
            }

            UE_LOG(LogTemp, Log, TEXT("Landing animation ended, all movement cleared"));
        }
    }
}


void UPlayerDefaultState::PlayLandingAnimation()
{
    if (LandingMontage == nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("LandingMontage is not set!"));
        return;
    }

    APlayerCharacter* aPlayer = Cast<APlayerCharacter>(mOwner);
    if (aPlayer == nullptr)
        return;

    USkeletalMeshComponent* MeshComp = aPlayer->FindComponentByClass<USkeletalMeshComponent>();
    if (MeshComp == nullptr)
        return;

    UAnimInstance* AnimInstance = MeshComp->GetAnimInstance();
    if (AnimInstance == nullptr)
        return;

    // 入力を無効化
    APlayerController* PC = GetWorld()->GetFirstPlayerController();
    if (PC)
    {
        mOwner->DisableInput(PC);
    }

    // モンタージュ再生
    float PlayLength = AnimInstance->Montage_Play(LandingMontage, 1.0f);
    if (PlayLength > 0.0f)
    {
        bIsPlayingLandingAnimation = true;
        UE_LOG(LogTemp, Log, TEXT("Landing animation started, duration: %f, input disabled"), PlayLength);
    }
}