// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/State/LadderClimberState.h"
#include "Player/State/StateManager.h"
#include "Player/PlayerCharacter.h"
#include "Objects/LadderActor.h"
#include "Components/MoveComponent.h"
#include "Kismet/KismetMathLibrary.h"
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

void ULadderClimberState::SetTargetLadder(const ALadderActor* ladderClimber)
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
		MoveComp->Init(mOwner, PlayerLogic);
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

	// 入力の取得（移動処理はこの関数では行わない）
	float Input = mOwner->GetInputAxisValue("MoveUp");

	// 上端と下端のワールド座標（Z軸）を取得
	const float LadderTopZ = Ladder->GetTopWorldPosition().Z;
	const float LadderBottomZ = Ladder->GetBottomWorldPosition().Z;
	const float PlayerZ = mOwner->GetActorLocation().Z;

	// 梯子の範囲外に出たらステート切り替え
	if (PlayerZ > LadderTopZ || PlayerZ < LadderBottomZ)
	{
		if (UStateManager* StateManager = mOwner->FindComponentByClass<UStateManager>())
		{
			StateManager->ChangeState("Default");
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
	// 移動方向をMoveCompのロジックから取得
	FVector direction = MoveComp->Movement(0, mOwner, Value);
	//// 速度は現在のステートが持つ移動速度を使用
	//mOwner->AddMovementInput(direction, 100000000);

	GetOwner()->AddActorLocalOffset(direction* 10, true);
}