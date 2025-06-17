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

// 毎フレームの更新
void UPhysicsCalculator::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// 物理計算が無効化されている場合（力の適用準備）
	if (!bIsPhysicsEnabled)
	{
		// 力の減衰（力が強すぎると減衰をかける）
		ForceScale = FMath::Max(ForceScale - DeltaTime * 10.0f, 0.0f); // DeltaTimeによって減衰させる

		// 力の方向とスケールで移動を反映
		GetOwner()->AddActorLocalOffset(ForceDirection * ForceScale, bIsSweep);

		// 現在の位置を取得
		FVector currentPosition = GetOwner()->GetActorLocation();
		float distanceZ = currentPosition.Z - PreviousPosition.Z;

		// 着地判定（地面に着いたら力をリセット）
		if (distanceZ < 0 && OnGround())
		{
			ForceDirection.Z = 0; // Z軸方向の力をリセット
			ForceScale = 0; // 力のスケールをゼロに
			bIsPhysicsEnabled = true; // 物理計算を有効に
			Velocity = FVector::ZeroVector; // 速度リセット
		}
		PreviousPosition = currentPosition;

		return;
	}

	// 重力を速度に加算（Z軸）
	Velocity.Z -= GravityScale * DeltaTime;

	// 速度を位置に反映
	GetOwner()->AddActorLocalOffset(Velocity * DeltaTime, true);

	PreviousPosition = GetOwner()->GetActorLocation();
}

// 力を加算する
void UPhysicsCalculator::AddForce(FVector Direction, float Force, const bool bSweep)
{
	// 既存の力に新しい力を加算
	ForceDirection = Direction; // 方向ベクトルを加算
	ForceScale = Force; // 力の強さを加算

	// スイープ衝突判定の設定
	bIsSweep = bSweep;

	// 物理計算を無効化して、新たな力を適用
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
void UPhysicsCalculator::AddGravity(const float gravityScale)
{
	if (OnGround())
	{
		Timer = 0;
		return;
	}

	Timer += GetWorld()->DeltaTimeSeconds;
	GetOwner()->AddActorLocalOffset(FVector(0, 0, -gravityScale) * Timer, true);
}

bool UPhysicsCalculator::OnGround() const
{
	AActor* Owner = GetOwner();
	if (!Owner) return false;

	// 1. 現在の位置とスケールを取得
	FVector ActorLocation = Owner->GetActorLocation();
	FVector ActorScale = Owner->GetActorScale();

	// 2. ボックスサイズ（スケールに基づいて拡張）
	FVector BoxExtent = FVector(20.0f * ActorScale.X, 20.0f * ActorScale.Y, 2.0f); // Zは浅く

	// 3. 足元の位置 = 中心から高さの半分を引く
	float HalfHeight = Owner->GetSimpleCollisionHalfHeight(); // Capsule などに対応
	FVector FootLocation = ActorLocation - FVector(0, 0, HalfHeight);

	// 4. レイの開始・終了位置（5ユニット下）
	FVector StartTrace = FootLocation;
	FVector EndTrace = FootLocation - FVector(0, 0, 5.0f);

	// 5. 衝突判定設定
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Owner);
	FHitResult Hit;

	// 6. Sweep（Box Trace）
	bool bHit = GetWorld()->SweepSingleByChannel(
		Hit,
		StartTrace,
		EndTrace,
		FQuat::Identity,
		ECC_Visibility,
		FCollisionShape::MakeBox(BoxExtent),
		Params
	);

	// 7. デバッグ表示
#if WITH_EDITOR
	DrawDebugBox(
		GetWorld(),
		StartTrace,
		BoxExtent,
		FQuat::Identity,
		bHit ? FColor::Green : FColor::Red,
		false, 1.0f
	);
#endif

	return bHit;
}
