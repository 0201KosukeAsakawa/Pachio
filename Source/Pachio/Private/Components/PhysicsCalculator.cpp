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

	// 力の強さが0以下の場合、力を加えない
	if (ForceScale <= 0)
		return;

	// 力の強さを減少させる（時間経過で徐々に減る）
	ForceScale = FMath::Max(0.0f, ForceScale - GetWorld()->DeltaTimeSeconds * 2.0f);

	// オーナー（このコンポーネントがアタッチされているアクター）に力を加える
	GetOwner()->AddActorLocalOffset(ForceDirection * ForceScale, bIsSweep);

	// 前回の位置と現在の位置を比較して、移動がほとんどない場合は力を0にする
	FVector currentPosition = GetOwner()->GetActorLocation();
	FVector direction = currentPosition - PreviousPosition;

	// 移動距離が非常に小さい場合は、力を0にして物理計算を停止
	if (direction.SizeSquared() <= FMath::Square(1.0f))
	{
		ForceScale = 0;
		bIsPhysicsEnabled = true; // 物理計算が完了したことを示す
		return;
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

// 重力をオブジェクトに加える
void UPhysicsCalculator::AddGravity()
{

	bool b = CanFall(GetOwner()->GetActorLocation(), GetOwner()->GetActorLocation());
	// オブジェクトが落下可能かどうかを判定
	if (b)
	{
		Timer = 0; // 落下できる場合はタイマーをリセット
		return;
	}

	// タイマーを進めて、重力を加える
	Timer += GetWorld()->DeltaTimeSeconds;
	GetOwner()->AddActorLocalOffset(FVector(0, 0, -9.8f) * Timer, true); // -9.8m/s²の重力を加える
}

// オブジェクトが落下可能かをチェックする
bool UPhysicsCalculator::CanFall(FVector Start, FVector End) const
{
	// Actor の位置を取得
	FVector ActorLocation = GetOwner()->GetActorLocation();

	// Actor のスケールを取得
	FVector ActorScale = GetOwner()->GetActorScale();

	// 足元の位置を計算（Z座標がActorの高さの半分の位置）
	FVector FeetPosition = ActorLocation;
	FeetPosition.Z -= ActorScale.Z * 50.0f; // 高さの半分を引いて足元の位置

	// 足元から下方向にレイを飛ばして衝突判定を行う
	FVector FallEnd = FeetPosition -FVector(0.0f, 0.0f, 10.0f); // 足元から下方向に1000ユニット

	// 衝突判定のためのパラメータ
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(GetOwner()); // このアクターを衝突判定から除外

	// 衝突結果を格納する変数
	FHitResult HitResult;

	// レイキャストで衝突判定を行う
	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, FeetPosition, FallEnd, ECC_Visibility, CollisionParams);

	// 衝突がない場合、落下できると判断
	return bHit;
}