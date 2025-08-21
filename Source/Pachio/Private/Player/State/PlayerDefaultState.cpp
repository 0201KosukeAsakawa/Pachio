// プロジェクト設定の Description ページに著作権情報を記入

#include "Player/State/PlayerDefaultState.h"
#include "Player/State/LadderClimberState.h"
#include "Player/PlayerCharacter.h"
#include "Player/InGameController.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/MoveComponent.h"
#include "Components/BoxComponent.h"
#include "Components/PhysicsCalculator.h"
#include "FunctionLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "InputActionValue.h"
#include "Interface/StateControllable.h"
#include "Interface/Soundable.h"
#include "Logic/Movement/PlayerMoveLogic.h"
#include "Manager/LevelManager.h"


#include "Objects/Color/LadderActor.h"
// 毎フレーム更新
#include "DrawDebugHelpers.h"  // これを忘れずに
UPlayerDefaultState::UPlayerDefaultState()
    : Direction(1), InitialRotationSet(false)
{
}

// ステート開始
bool UPlayerDefaultState::OnEnter(ACharacter* owner, UWorld* world)
{
    if (!owner || !world) return false;

    mOwner = owner;
    pWorld = world;

    // マテリアル設定（省略可）
    if (UStaticMeshComponent* MeshComp = UFunctionLibrary::FindComponentByName<UStaticMeshComponent>(owner, "StaticMesh"))
    {
        if (UMaterialInterface* Mat = NewMaterial.LoadSynchronous())
        {
            MeshComp->SetMaterial(0, Mat);
        }
    }

    // コリジョンサイズ変更
    mOwner->GetCharacterMovement()->Crouch();
    mOwner->GetCapsuleComponent()->SetCapsuleHalfHeight(55.0f);

    // 移動速度
    mMoveSpeed = 500.0f;

    // Direction 初期値
    Direction = 1;

    // BoxComponent をキャッシュ
    BoxComp = UFunctionLibrary::FindComponentByName<UBoxComponent>(GetOwner(), TEXT("Box"));

    // 初期回転をキャッシュ
    InitialRotation = mOwner->GetActorRotation();
    InitialRotationSet = true;

    return true;
}



bool UPlayerDefaultState::OnUpdate(float DeltaTime)
{
    if (!mOwner || !BoxComp) return false;
    UWorld* World = mOwner->GetWorld();
    if (!World) return false;

    FVector Start = mOwner->GetActorLocation();
    FVector DownEnd = Start - (FVector(0, 0, 700.0f));

    // --- 床レイ ---
    FHitResult FloorHit;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(mOwner);
    bool bOnGround = World->LineTraceSingleByChannel(FloorHit, Start, DownEnd, ECC_Visibility, Params);

    // 床レイを可視化（緑）
    DrawDebugLine(World, Start, DownEnd, FColor::Green, false, 0.1f, 0, 2.0f);
    if (bOnGround)
    {
        DrawDebugSphere(World, FloorHit.ImpactPoint, 5.0f, 12, FColor::Yellow, false, 0.1f);
    }

    FVector MoveVec = FVector(0, 1, 0) * Direction;
    if (bOnGround)
    {
        MoveVec = FVector::VectorPlaneProject(MoveVec, FloorHit.ImpactNormal).GetSafeNormal();
    }

    // --- 前方レイ ---
    FHitResult ForwardHit;
    FVector ForwardEnd = Start + MoveVec * 200.0f;
    bool bHitWall = World->LineTraceSingleByChannel(ForwardHit, Start, ForwardEnd, ECC_Visibility, Params);

    // 前方レイを可視化（赤）
    DrawDebugLine(World, Start, ForwardEnd, FColor::Red, false, 0.1f, 0, 2.0f);
    if (bHitWall)
    {
        DrawDebugSphere(World, ForwardHit.ImpactPoint, 5.0f, 12, FColor::Blue, false, 0.1f);
    }

    // 以下、移動や傾きの処理は元のコードと同じ
    if (bHitWall)
    {
        FVector ForwardDir = MoveVec.GetSafeNormal();
        FVector WallNormal = ForwardHit.ImpactNormal.GetSafeNormal();

        // Dot から角度（度数法）を計算
        float AngleDegrees = FMath::RadiansToDegrees(acosf(FVector::DotProduct(ForwardDir, -WallNormal)));
        float Angle = FMath::RadiansToDegrees(acosf(FVector::DotProduct(MoveVec.GetSafeNormal(), -ForwardHit.ImpactNormal.GetSafeNormal())));
        const float MaxSlopeAngle = 60.0f;
        if (Angle <= 0)
        {
            Direction *= -1;
            return true;
        }
        else
        {
            FVector SlideDelta = FVector::VectorPlaneProject(MoveVec * mMoveSpeed * DeltaTime, ForwardHit.ImpactNormal);
            FHitResult SlideHit;
            mOwner->GetRootComponent()->MoveComponent(SlideDelta, mOwner->GetActorRotation(), true, &SlideHit);
            return true;
        }
    }

    FVector MoveDelta = MoveVec * mMoveSpeed * DeltaTime;
    FHitResult Hit;
    mOwner->GetRootComponent()->MoveComponent(MoveDelta, mOwner->GetActorRotation(), true, &Hit);

    if (bOnGround)
    {
        FVector NewLoc = mOwner->GetActorLocation();
        NewLoc.Z = FloorHit.ImpactPoint.Z + BoxComp->GetScaledBoxExtent().Z;

        FRotator AlignRot = UKismetMathLibrary::MakeRotFromXZ(MoveVec, FloorHit.ImpactNormal);
        mOwner->SetActorRotation(AlignRot);
    }
    else if (InitialRotationSet)
    {
        mOwner->SetActorRotation(InitialRotation);
    }

    return true;
}



// ステート終了
bool UPlayerDefaultState::OnExit(ACharacter*)
{
    return true;
}

// スキル入力
bool UPlayerDefaultState::OnSkill(const FInputActionValue& Value)
{
    if (Value.Get<bool>())
    {
        if (AController* OwningController = mOwner->GetController())
        {
            if (AInGameController* InGameController = Cast<AInGameController>(OwningController))
            {
                InGameController->TogglePossession(mOwner);
            }
        }
    }
    return true;
}

// 外部入力（オート移動なので空）
void UPlayerDefaultState::Movement(const FInputActionValue& Value) {}

void UPlayerDefaultState::Jump(UPhysicsCalculator* physics,float jumpForce)
{
    if (GetOwner() == nullptr || physics == nullptr)
        return;

    if (TryEnterLadderOnJump())
    {
        physics->SetGravityScale(false);
        return;
    }

    if (!physics || !physics->OnGround())
        return;

    // ジャンプ力を掛けて力を加える
    physics->AddForce(GetOwner()->GetActorUpVector(), jumpForce);
    ISoundable* sound = ALevelManager::GetInstance(GetWorld())->GetSoundManager().GetInterface();
    sound->PlaySound("SE", "Jump");
}

bool UPlayerDefaultState::TryEnterLadderOnJump() const
{
    if (mOwner == nullptr)
        return false;
    ACharacter* owner = Cast<ACharacter>(mOwner);
    if (owner == nullptr)
        return false;

    FVector Start = mOwner->GetActorLocation();
    FVector End = Start + FVector(0, 0, 100.f);
    FVector BoxHalfExtent = FVector(30.f, 30.f, 100.f);

    FCollisionQueryParams Params;
    Params.AddIgnoredActor(mOwner);

    TArray<FHitResult> Hits;
    bool bAnyHit = GetWorld()->SweepMultiByChannel(
        Hits,
        Start,
        End,
        FQuat::Identity,
        ECC_Visibility, // カスタムチャンネルでも可
        FCollisionShape::MakeBox(BoxHalfExtent),
        Params
    );

    if (!bAnyHit)
        return false;
    IStateControllable* player = Cast<IStateControllable>(GetOwner());
    if (player == nullptr)
        return false;

    for (const FHitResult& Hit : Hits)
    {
        if (!Hit.GetActor() || !Hit.GetActor()->ActorHasTag("Ladder"))
            continue;

        ALadderActor* Ladder = Cast<ALadderActor>(Hit.GetActor());
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