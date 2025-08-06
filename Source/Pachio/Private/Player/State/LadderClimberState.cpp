// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/State/LadderClimberState.h"
#include "Player/State/StateManager.h"
#include "Player/PlayerCharacter.h"
#include "Objects/Color/LadderActor.h"
#include "Components/MoveComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/OverlapResult.h"
#include "Logic/Movement/LadderMoveLogic.h"


FVector GetLadderCenterXZ(const AActor* Ladder)
{
	if (!Ladder) return FVector::ZeroVector;

	// バウンディングボックスを取得（ワールド空間）
	FVector Origin;
	FVector BoxExtent;
	Ladder->GetActorBounds(true, Origin, BoxExtent);

	// Origin はワールド空間で中心位置
	// X,Y はそのまま使い、Zは無視（XZ中心だけほしいので）

	return FVector(Origin.X, Origin.Y, 0.f);
}

void ULadderClimberState::SetTargetLadder(ALadderActor* ladderClimber)
{
	if (ladderClimber == nullptr)
		return;
	Ladder = ladderClimber;
}

bool ULadderClimberState::OnEnter(ACharacter* Owner, UWorld* World)
{
	if (!Owner) return false;
	if (!mOwner)
		mOwner = Owner;
	if (!pWorld)
		pWorld = World;
	if (!MoveComp)
	{
		MoveComp = NewObject<UMoveComponent>(mOwner);
		ULadderMoveLogic* PlayerLogic = NewObject<ULadderMoveLogic>(this);
		MoveComp->Init(PlayerLogic);
	}

	// 梯子の中心に位置補正（必要なら LadderActor を別途持っておく）
	if (Ladder)
	{
		// 位置補正処理
		FVector Center = GetLadderCenterXZ(Ladder);
		FVector OwnerLocation = Owner->GetActorLocation();

		// 新しい位置は梯子の中心のX,Yを使い、Zはキャラの現在の高さを維持
		FVector NewLocation = FVector(Center.X, Center.Y, OwnerLocation.Z);

		Owner->SetActorLocation(NewLocation);
	}

	return true;
}


bool ULadderClimberState::OnUpdate(float DeltaTime)
{
	if (!mOwner || !Ladder) return false;

	float Input = mOwner->GetInputAxisValue("MoveUp");

	const float LadderTopZ = Ladder->GetTopWorldPosition().Z;
	const float LadderBottomZ = Ladder->GetBottomWorldPosition().Z;
	const float PlayerZ = mOwner->GetActorLocation().Z;

	if (PlayerZ > LadderTopZ || PlayerZ < LadderBottomZ)
	{
		// 梯子の外に出た → 周囲に別の梯子があるか調べる
		const FVector PlayerLocation = mOwner->GetActorLocation();
		const FVector BoxExtent(20.0f, 20.0f, 50.0f); // 小さなボックス
		TArray<FOverlapResult> Overlaps;

		FCollisionQueryParams Params;
		Params.AddIgnoredActor(mOwner);
		if (AActor* CurrentLadderActor = Cast<AActor>(Ladder))
		{
			Params.AddIgnoredActor(CurrentLadderActor);
		}

		bool bFoundNewLadder = false;

		UWorld* World = mOwner->GetWorld();
		if (World && World->OverlapMultiByObjectType(
			Overlaps,
			PlayerLocation,
			FQuat::Identity,
			FCollisionObjectQueryParams(ECollisionChannel::ECC_WorldDynamic),
			FCollisionShape::MakeBox(BoxExtent),
			Params))
		{
			for (const FOverlapResult& Result : Overlaps)
			{
				if (AActor* HitActor = Result.GetActor())
				{
					// 現在のLadderとは別で、Ladderクラスか派生クラスなら
					if (HitActor->IsA(Ladder->GetClass()))
					{
						// 新しい梯子に切り替え
						Ladder = Cast<ALadderActor>(HitActor); // ALadder* にキャストする場合は適宜修正
						bFoundNewLadder = true;
						break;
					}
				}
			}
		}

		if (!bFoundNewLadder)
		{
			// 近くに他の梯子がない → 通常状態へ戻す
			if (UStateManager* StateManager = mOwner->FindComponentByClass<UStateManager>())
			{
				StateManager->ChangeState("Default");
			}
		}
	}

	return true;
}


bool ULadderClimberState::OnExit(ACharacter* Owner)
{
	if (!Owner) return false;

	APlayerCharacter* player = Cast<APlayerCharacter>(Owner);
	if (player == nullptr)
		return false;

	player->SetGravityScale(true);
	return true;
}

bool ULadderClimberState::OnSkill(const FInputActionValue& Input)
{
	IStateControllable* owner = Cast<IStateControllable>(mOwner);
	if (owner == nullptr)
		return false;

	return owner->ChangeState("Default");
}

void ULadderClimberState::Movement(const FInputActionValue& Value)
{
	if (!mOwner || !Ladder) return;

	FVector direction = MoveComp->Movement(0, mOwner, Value);

	// ★ 梯子の前後判定 ★
	FVector LadderForward = Ladder->GetActorForwardVector();
	FVector ToPlayer = mOwner->GetActorLocation() - Ladder->GetActorLocation();
	ToPlayer.Normalize();

	float Dot = FVector::DotProduct(LadderForward, ToPlayer);

	// 後ろ側にいるなら方向を反転
	if (Dot < 0.f)
	{
		direction *= -1.f;
	}

	FVector NewLocation = mOwner->GetActorLocation() + direction * 10;

	// 下方向の移動なら地面チェック
	if (direction.Z < 0.f)
	{
		FVector Start = NewLocation;
		FVector End = Start - FVector(0.f, 0.f, 100.f);  // 100cm下方向

		FHitResult HitResult;
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(mOwner);

		UWorld* World = mOwner->GetWorld();
		bool bHit = false;
		if (World)
		{
			bHit = World->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, Params);
		}

		if (bHit)
		{
			if (IStateControllable* owner = Cast<IStateControllable>(mOwner))
			{
				owner->ChangeState("Default");
				return;
			}
		}
	}

	// 通常の移動
	GetOwner()->AddActorLocalOffset(direction * 10, true);
}
