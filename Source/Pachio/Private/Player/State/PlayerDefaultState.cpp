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
bool UPlayerDefaultState::OnEnter(ACharacter* owner, UWorld* world)
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

   
    if (!BoxComponent)
    {
        BoxComponent = UFunctionLibrary::FindComponentByName<UBoxComponent>(GetOwner(), TEXT("Box"));
    }
      

    // キャラクターが持つ StaticMeshComponent を取得
    UStaticMeshComponent* StaticMeshComp = UFunctionLibrary::FindComponentByName<UStaticMeshComponent>(owner, "StaticMesh");
    UMaterialInterface* N = NewMaterial.LoadSynchronous(); // 非同期ロードに対応
    if (N != nullptr && StaticMeshComp)
    {
        StaticMeshComp->SetMaterial(0, N); // マテリアルをスロット0に適用
    }


    //コリジョンのサイズ変更
    mOwner->GetCharacterMovement()->Crouch();
    mOwner->GetCapsuleComponent()->SetCapsuleHalfHeight(55.0);

    APlayerCharacter* aPlayer = Cast<APlayerCharacter>(mOwner);
    if (!aPlayer)
        return false;


    // 移動速度の初期値設定（ステート内で使用）
    mMoveSpeed = 100.0f;
    CurrentDirection = mOwner->GetActorForwardVector();

    GetWorld()->GetTimerManager().SetTimer(
        CheckHoldableHandle,
        this,
        &UPlayerDefaultState::UpdateInteractableUI,
        0.1f,   // 間隔
        true    // 繰り返し
    );

    return true; // ステートの切り替え成功
}

// ステートの毎フレーム更新処理（現時点では何もしない）
bool UPlayerDefaultState::OnUpdate(float)
{
    if (GetWorld() == nullptr || Physics == nullptr)
        return false;

    if (Physics->HasLanded())
    {
        ISoundable* sound = ALevelManager::GetInstance(GetWorld())->GetSoundManager().GetInterface();
        if (sound)
        {
            sound->PlaySound("SE", "Land"); // ←着地音に名称変更
        }
    }

    return true;
}

// ステートを離脱するときの処理（現時点では何もしない）
bool UPlayerDefaultState::OnExit(ACharacter*)
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
    UPlayerStateComponent* NewState = Player->ChangeState("Hold");
    if (NewState == nullptr)
        return false;

    if (UPlayerHoldState* HoldState = Cast<UPlayerHoldState>(NewState))
    {
        HoldState->SetUp(Target);
    }

    return true;
}

void UPlayerDefaultState::Movement(const FInputActionValue& Value)
{
    FVector2D MoveInput = Value.Get<FVector2D>();

    float DeadZone = 0.2f;
    if (MoveInput.X >= DeadZone && TryEnterLadderOnJump())
    {
        Physics->AddForce(FVector(0, 0, 0), 0.f);
        Physics->SetGravityScale(false);
        return;
    }

    // 移動方向をMoveCompのロジックから取得
    FVector direction = MoveComp->Movement(0, mOwner, Value);
    // 速度は現在のステートが持つ移動速度を使用
    mOwner->AddMovementInput(direction, MoveSpeed);
    if (direction != FVector(0, 0, 0))
        CurrentDirection = direction;
}

void UPlayerDefaultState::Jump(float jumpForce)
{
    if (GetOwner() == nullptr || Physics == nullptr || !Physics->OnGround())
        return;

    // ジャンプ力を掛けて力を加える
    Physics->AddForce(GetOwner()->GetActorUpVector(), jumpForce);
    ISoundable* sound = ALevelManager::GetInstance(GetWorld())->GetSoundManager().GetInterface();
    sound->PlaySound("SE", "Jump");
}

bool UPlayerDefaultState::TryEnterLadderOnJump() const
{
    if (mOwner == nullptr)
        return false;

    // プレイヤーにアタッチされたBoxComponentを用意している想定
    // 例えば LadderCheckTrigger として UBoxComponent* を保持している
    if (BoxComponent == nullptr)
        return false;

    TArray<AActor*> OverlappingActors;
    BoxComponent->GetOverlappingActors(OverlappingActors, ALadderActor::StaticClass());

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
        if (UPlayerStateComponent* NewState = player->ChangeState("Climb"))
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

void UPlayerDefaultState::UpdateInteractableUI()
{
    CheckHoldableObject();
    CheckLadderObject();
}

void UPlayerDefaultState::CheckHoldableObject()
{
    FVector Start = mOwner->GetActorLocation();
    FVector End = Start + CurrentDirection * 200.f;

    // LineTrace にしたいなら BoxShape を小さくして代用してもOK
    FCollisionShape Shape = FCollisionShape::MakeSphere(5.f);

    CheckObjectByTag(Start, End, Shape, "Holdable", bPrevCanHold, "HaveUI", "HaveUIAnimation");
}

void UPlayerDefaultState::CheckLadderObject()
{
    FVector Start = mOwner->GetActorLocation();
    FVector End = Start + FVector(0, 0, 100.f);
    FCollisionShape Shape = FCollisionShape::MakeBox(FVector(30.f, 30.f, 100.f));

    CheckObjectByTag(Start, End, Shape, "Ladder", bPrevCanClim, "HaveUI", "LadderUIAnimation");
}


bool UPlayerDefaultState::CheckObjectByTag(
    const FVector& Start,
    const FVector& End,
    const FCollisionShape& Shape,
    const FName& Tag,
    bool& bPrevState,
    const FName& WidgetName,
    const FName& AnimName
)
{
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(mOwner);

    TArray<FHitResult> Hits;
    bool bAnyHit = GetWorld()->SweepMultiByChannel(
        Hits,
        Start,
        End,
        FQuat::Identity,
        ECC_Visibility,
        Shape,
        Params
    );

    bool bHit = false;
    if (bAnyHit)
    {
        for (const FHitResult& Hit : Hits)
        {
            if (Hit.GetActor() && Hit.GetActor()->ActorHasTag(Tag))
            {
                bHit = true;
                break;
            }
        }
    }

    if (bPrevState == bHit)
        return bHit; // 状態変化なし

    const ALevelManager* LevelManager = ALevelManager::GetInstance(GetWorld());
    if (LevelManager)
    {
        if (UUIManager* UIManager = LevelManager->GetUIManager())
        {
            if (bHit)
            {
                UIManager->ShowWidget(EWidgetCategory::Tutorial, WidgetName);
                UIManager->PlayWidgetAnimation(EWidgetCategory::Tutorial, WidgetName, AnimName);
            }
            else
            {
                UIManager->HideCurrentWidget(EWidgetCategory::Tutorial, WidgetName);
            }
        }
    }

    bPrevState = bHit;
    return bHit;
}