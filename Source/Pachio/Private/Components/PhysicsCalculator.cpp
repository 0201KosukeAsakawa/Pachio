#include "Components/PhysicsCalculator.h"
#include "Math/UnrealMathUtility.h"
#include "DrawDebugHelpers.h"

// コンストラクタでデフォルト値を設定
UPhysicsCalculator::UPhysicsCalculator()
	: ForceScale(0)
	, ForceDirection(FVector::ZeroVector)
	, PreviousPosition(FVector::ZeroVector)
	, Timer(0)
	, bShouldApplyGravity(true)
	, bIsSweep(false)
	, bIsPhysicsEnabled(false)
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UPhysicsCalculator::BeginPlay()
{
	Super::BeginPlay();
}

// 毎フレーム更新処理
void UPhysicsCalculator::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bShouldApplyGravity)
		AddGravity();
	FVector MoveVector;

	if (!bIsPhysicsEnabled)
	{
		ForceScale = FMath::Max(ForceScale - DeltaTime * 10.0f, 0.0f);
		MoveVector = ForceDirection * ForceScale;

		FVector Adjusted = GetBlockedAdjustedVector(MoveVector);
		GetOwner()->AddActorLocalOffset(Adjusted, bIsSweep);

		FVector currentPosition = GetOwner()->GetActorLocation();
		float distanceZ = currentPosition.Z - PreviousPosition.Z;

		if (distanceZ < 0 && OnGround())
		{
			ForceDirection.Z = 0;
			ForceScale = 0;
			bIsPhysicsEnabled = true;
			Velocity = FVector::ZeroVector;
		}
		PreviousPosition = currentPosition;

		return;
	}
}

void UPhysicsCalculator::AddForce(FVector Direction, float Force, const bool bSweep)
{
	ForceDirection = Direction;
	ForceScale = Force /** ForceModifier*/;
	Timer = 0;
	bIsSweep = bSweep;
	bIsPhysicsEnabled = false;
}

void UPhysicsCalculator::ResetForce()
{
	ForceDirection = FVector::ZeroVector;
	ForceScale = 0;
	Timer = 0;
	bIsPhysicsEnabled = true;
}

void UPhysicsCalculator::AddGravity()
{
	if (OnGround())
	{
		Timer = 0;
		return;
	}

	Timer += GetWorld()->DeltaTimeSeconds;
	GetOwner()->AddActorLocalOffset((FVector(0, 0, -GravityScale) * Timer)/ ForceModifier, true);
}

bool UPhysicsCalculator::OnGround() const
{
	AActor* Owner = GetOwner();
	if (!Owner) return false;

	FVector ActorLocation = Owner->GetActorLocation();
	FVector ActorScale = Owner->GetActorScale();
	FVector BoxExtent(20.0f * ActorScale.X, 20.0f * ActorScale.Y, 2.0f); // 薄い足元のボックス

	float HalfHeight = Owner->GetSimpleCollisionHalfHeight();

	// オーナーの回転を取得
	const FQuat ActorRotation = Owner->GetActorQuat();

	// 回転を考慮して足元の位置を計算
	FVector DownVector = ActorRotation.GetUpVector() * -1.f;
	FVector FootLocation = ActorLocation + DownVector * HalfHeight;

	FVector StartTrace = FootLocation;
	FVector EndTrace = FootLocation + DownVector * 5.0f; // 足元の下方向へ5cmトレース

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Owner);

	FHitResult Hit;

	bool bHit = GetWorld()->SweepSingleByChannel(
		Hit,
		StartTrace,
		EndTrace,
		ActorRotation, // ★回転を考慮したボックスの方向
		ECC_Visibility,
		FCollisionShape::MakeBox(BoxExtent),
		Params
	);

#if WITH_EDITOR
	DrawDebugBox(
		GetWorld(),
		StartTrace,
		BoxExtent,
		ActorRotation, // ★ここも回転を反映
		bHit ? FColor::Green : FColor::Red,
		false, 1.0f
	);
#endif

	return bHit;
}


void UPhysicsCalculator::SetGravityScale(bool applyGravity, float scale,float Modifier)
{
	GravityScale = scale;
	bShouldApplyGravity = applyGravity;
	ForceModifier = Modifier;
}

FVector UPhysicsCalculator::GetBlockedAdjustedVector(const FVector& MoveVector)
{
	AActor* Owner = GetOwner();
	if (!Owner || MoveVector.IsNearlyZero()) return MoveVector;

	FVector ActorLocation = Owner->GetActorLocation();
	FVector ActorScale = Owner->GetActorScale();

	// スケールを元にした固定サイズ（基準20cm）で BoxExtent を計算
	FVector BoxExtent(
		20.0f * ActorScale.X,  // 横幅
		20.0f * ActorScale.Y,  // 奥行き（前後）
		20.0f * ActorScale.Z   // 高さ
	);

	FVector Direction = MoveVector.GetSafeNormal();
	const float BackstepDistance = 1.0f;
	FVector Start = ActorLocation - Direction * BackstepDistance;
	FVector End = Start + MoveVector;

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Owner);

	bool bHit = GetWorld()->SweepSingleByChannel(
		Hit,
		Start,
		End,
		FQuat::Identity,
		ECC_Visibility,
		FCollisionShape::MakeBox(BoxExtent),
		Params
	);

	if (bHit)
	{
		const float AdjustMargin = 0.1f;
		float Distance = FMath::Max(Hit.Distance - AdjustMargin, 0.0f);
		return Direction * Distance;
	}

	return MoveVector;
}
FVector UPhysicsCalculator::GetGroundNormal() const
{
	AActor* Owner = GetOwner();
	if (!Owner) return FVector::UpVector;

	FVector ActorLocation = Owner->GetActorLocation();
	FVector ActorScale = Owner->GetActorScale();
	FVector BoxExtent(20.0f * ActorScale.X, 20.0f * ActorScale.Y, 2.0f);

	float HalfHeight = Owner->GetSimpleCollisionHalfHeight();
	FVector FootLocation = ActorLocation - FVector(0, 0, HalfHeight);
	FVector StartTrace = FootLocation;
	FVector EndTrace = FootLocation - FVector(0, 0, 5.0f);

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Owner);
	FHitResult Hit;

	bool bHit = GetWorld()->SweepSingleByChannel(
		Hit,
		StartTrace,
		EndTrace,
		FQuat::Identity,
		ECC_Visibility,
		FCollisionShape::MakeBox(BoxExtent),
		Params
	);

	if (bHit)
	{
		return Hit.Normal;
	}

	// 接地してなければ上向きを返す
	return FVector::UpVector;
}