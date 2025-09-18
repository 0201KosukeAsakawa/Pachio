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

	FVector Origin;
	FVector BoxExtent;
	Ladder->GetActorBounds(true, Origin, BoxExtent);

	// XYの中心位置を返す。Zは呼び出し元で設定する想定
	return FVector(Origin.X, Origin.Y, 0.f);
}

void ULadderClimberState::SetTargetLadder(ALadderActor* ladderClimber)
{
	if (ladderClimber == nullptr)
		return;
	Ladder = ladderClimber;

	if (Ladder)
	{
		p = Ladder->GetFixedPositionForActor(GetOwner());
		GetOwner()->SetActorLocation(p);
	}

}

bool ULadderClimberState::OnEnter(APawn* Owner, UWorld* World)
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
	return true;
}


bool ULadderClimberState::OnUpdate(float DeltaTime)
{
	if (!mOwner || !Ladder) return false;

	if (Ladder)
	{
		p = Ladder->GetFixedPositionForActor(GetOwner());
		
		FVector OwnerLocation = GetOwner()->GetActorLocation();
		// X,Yは梯子の中心、Zはキャラの現在位置のまま
		FVector NewLocation = FVector(p.X, p.Y, OwnerLocation.Z);
		GetOwner()->SetActorLocation(NewLocation);
	}

	if (Ladder->IsHidden())
	{
		// Hold解除して Default に戻す
		if (IStateControllable* Player = Cast<IStateControllable>(mOwner))
		{
			Player->ChangeState(EPlayerStateType::Default);
		}
		return true;
	}

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
				if (PlayerZ > LadderTopZ)
				{
					float direction = 1;
					// ★ 梯子の前後判定 ★
					FVector LadderForward = Ladder->GetActorForwardVector();
					FVector ToPlayer = mOwner->GetActorLocation() - Ladder->GetActorLocation();
					ToPlayer.Normalize();

					float Dot = FVector::DotProduct(Ladder->GetActorRightVector(), ToPlayer);

					// 後ろ側にいるなら方向を反転
					if (Dot < 0.f)
					{
						direction *= -1.f;
					}
				}

				// 状態を戻す
				StateManager->ChangeState(EPlayerStateType::Default);
			}

		}
	}

	return true;
}


bool ULadderClimberState::OnExit(APawn* Owner)
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

	return owner->ChangeState(EPlayerStateType::Default) != nullptr;
}

void ULadderClimberState::Movement(const FInputActionValue& Value)
{
	if (!mOwner || !Ladder) return;

	FVector direction = MoveComp->Movement(0, mOwner, Value);

	// 移動方向を入力から決定
	if (direction.Z < 0)
	{
		MoveDirection = -1;
	}
	else if (direction.Z > 0)
	{
		MoveDirection = 1;
	}
	else
	{
		MoveDirection = 0;
	}

	FVector NewLocation = mOwner->GetActorLocation() + direction * 10;

	// 下方向の移動なら地面チェック
	if (direction.Z < 0.f)
	{
		FVector Start = NewLocation;
		FVector End = Start - FVector(0.f, 0.f, 300.f);  // 100cm下方向

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
				owner->ChangeState(EPlayerStateType::Default);
				return;
			}
		}
	}
	MoveDelta = direction * 10;
	// 通常の移動
	GetOwner()->AddActorLocalOffset(direction * 10, true);
	p = Ladder->GetFixedPositionForActor(GetOwner());
	GetOwner()->SetActorLocation(p);
}
