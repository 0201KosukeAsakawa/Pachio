// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/PhysicsCalculator.h"
#include "Math/UnrealMathUtility.h"

// コンストラクタでデフォルト値を設定
UPhysicsCalculator::UPhysicsCalculator()
	: GravityScale(0) // 重力スケール（デフォルトは0）
	, ForceScale(0) // 力のスケール（デフォルトは0）
	, ForceDirection(FVector(0, 0, 0)) // 力を加える方向（デフォルトはゼロベクトル）
	, PreviousPosition(FVector(0, 0, 0)) // 前回の位置（デフォルトはゼロベクトル）
	, Timer(0) // タイマー（デフォルトは0）
	, bShouldApplyGravity(true) // 重力を加えるかどうか（デフォルトはtrue）
	, bIsSweep(false) // スイープ衝突判定を使用するか（デフォルトはfalse）
	, bIsPhysicsEnabled(false) // 物理計算が有効かどうか（デフォルトはfalse）
{
	// このコンポーネントは毎フレームTickします
	PrimaryComponentTick.bCanEverTick = true;
}

// ゲーム開始時に呼ばれる
void UPhysicsCalculator::BeginPlay()
{
	Super::BeginPlay(); // 親クラスのBeginPlayを呼び出す
}

// 毎フレーム呼ばれる
void UPhysicsCalculator::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction); // 親クラスのTickComponentを呼び出す
	if (bIsPhysicsEnabled)
		return;

	// 力の強さを減少させる（時間経過で徐々に減る）
	ForceScale = ForceScale - GetWorld()->DeltaTimeSeconds;

	// オーナー（このコンポーネントがアタッチされているアクター）に力を加える
	GetOwner()->AddActorLocalOffset(ForceDirection * ForceScale, bIsSweep);

	// 前回の位置と現在の位置を比較して、移動がほとんどない場合は力を0にする
	FVector currentPosition = GetOwner()->GetActorLocation();

	FVector direction = currentPosition - PreviousPosition;
	
	float distance = GetOwner()->GetActorLocation().Z - PreviousPosition.Z;

	if (distance < 0 && OnGround(GetOwner()->GetActorLocation(), FVector(GetOwner()->GetActorLocation().X, GetOwner()->GetActorLocation().Y, GetOwner()->GetActorLocation().Z - 10)))
	{
		ForceDirection.Z = 0;
		ForceScale = 0;
		bIsPhysicsEnabled = true;
	}
	// 現在の位置を記録して次回の比較に使う
	PreviousPosition = currentPosition;
}

// 指定した方向に力を加える
void UPhysicsCalculator::AddForce(FVector Direction, float Force,const bool bSweep)
{
	ForceDirection = Direction; // 力の方向を設定
	ForceScale = Force; // 力のスケール（強さ）を設定
	Timer = 0; // タイマーをリセット
	bIsSweep = bSweep; // スイープ衝突判定を設定
	bIsPhysicsEnabled = false;
}

void UPhysicsCalculator::ResetForce()
{
	ForceDirection = FVector(0,0,0); // 力の方向を設定
	ForceScale = 0; // 力のスケール（強さ）を設定
	Timer = 0;
	bIsPhysicsEnabled = true;
}

// 重力をオブジェクトに加える
void UPhysicsCalculator::AddGravity(const float gravityScalse)
{
	// オブジェクトが落下可能かどうかを判定
	if (OnGround(GetOwner()->GetActorLocation(), GetOwner()->GetActorLocation()))
	{
		Timer = 0; // 落下できる場合はタイマーをリセット
		return;
	}

	// タイマーを進めて、重力を加える
	Timer += GetWorld()->DeltaTimeSeconds;
	GetOwner()->AddActorLocalOffset(FVector(0, 0, -gravityScalse) * Timer, true); // -9.8m/s²の重力を加える
}

bool UPhysicsCalculator::OnGround(FVector Start, FVector End) const
{
	FVector ActorLocation = GetOwner()->GetActorLocation();

	// 足元のボックスサイズ（横は広め、縦は浅め）
	FVector BoxExtent = FVector(20.0f, 20.0f, 2.0f);

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(GetOwner());

	FHitResult Hit;

	// Actor の足元の位置に設定
	FVector StartTrace = ActorLocation;
	StartTrace.Z -= GetOwner()->GetActorScale().Z * 50.0f;

	// 少しだけ下に移動した位置にトレース
	FVector EndTrace = StartTrace - FVector(0.0f, 0.0f, 5.0f);

	// Sweep (BoxTrace)
	bool bHit = GetWorld()->SweepSingleByChannel(
		Hit,
		StartTrace,
		EndTrace,
		FQuat::Identity,
		ECC_Visibility,
		FCollisionShape::MakeBox(BoxExtent),
		Params
	);

	DrawDebugBox(
		GetWorld(),
		StartTrace,
		BoxExtent,
		FQuat::Identity,
		bHit ? FColor::Green : FColor::Red,
		false, 1.0f
	);


	return bHit; // ヒットしなければ「落下可能」
}
