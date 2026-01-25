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

	AActor* Owner = GetOwner();

	if (IsActive())
	{
		return;
	}
	if (bShouldApplyGravity)
	{
		AddGravity();
		UpdateGroundState();
	}

	// ★ 修正: 接地したら上昇力をキャンセル（ただしジャンプ直後は無視）
	if (!bIgnoreGroundCheck && OnGround() && ForceDirection.Z > 0)
	{
		ForceDirection.Z = 0;
		ForceScale = 0;
		bIsPhysicsEnabled = true;
		Timer = 0;
	}

	// ★ 追加: 地面から離れたらフラグを解除
	if (bIgnoreGroundCheck && !OnGround())
	{
		bIgnoreGroundCheck = false;
	}

	FVector MoveVector;
	if (!bIsPhysicsEnabled)
	{
		ForceScale = FMath::Max(ForceScale - DeltaTime * 10.0f, 0.0f);
		MoveVector = ForceDirection * ForceScale;

		FVector Adjusted = GetBlockedAdjustedVector(MoveVector);

		if (ForceDirection.Z > 0 && Adjusted.Z < MoveVector.Z * 0.1f)
		{
			ForceDirection.Z = 0;
			ForceScale = 0;
			bIsPhysicsEnabled = true;
		}

		if (bUseLocalOffset)
		{
			GetOwner()->AddActorLocalOffset(Adjusted, bIsSweep);
		}
		else
		{
			GetOwner()->AddActorWorldOffset(Adjusted, bIsSweep);
		}

		FVector currentPosition = GetOwner()->GetActorLocation();
		float distanceZ = currentPosition.Z - PreviousPosition.Z;

		if (distanceZ < 0)
		{
			ForceDirection.Z = 0;
			ForceScale = 0;
			Timer = 0;
			bIsPhysicsEnabled = true;
			bFalling = true;
		}

		PreviousPosition = currentPosition;
	}
}

void UPhysicsCalculator::UpdateGroundState()
{
	bool bIsCurrentlyOnGround = OnGround();
	bHasJustLanded = (!bWasOnGround && bIsCurrentlyOnGround);

	if (bHasJustLanded)
		UE_LOG(LogTemp, Log, TEXT("着地"));

	bWasOnGround = bIsCurrentlyOnGround;
}

void UPhysicsCalculator::AddForce(FVector Direction, float Force, const bool bSweep, const bool useLocalOffset)
{
	ForceDirection = Direction;
	ForceScale = Force;
	Timer = 0;
	bIsSweep = bSweep;
	bIsPhysicsEnabled = false;
	bUseLocalOffset = useLocalOffset;

	// ★ 追加: 上向きの力(ジャンプ)の場合、一時的に接地判定を無視
	if (Direction.Z > 0)
	{
		bIgnoreGroundCheck = true;
	}
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
		bIsPhysicsEnabled = false;
		bFalling = false;
		Timer = 0;
		return;
	}

	Timer += GetWorld()->DeltaTimeSeconds;

	// 落下速度を計算
	float FallSpeed = (GravityScale * Timer) / ForceModifier;

	// 上限を適用
	FallSpeed = FMath::Min(FallSpeed, MaxFallingSpeed);

	GetOwner()->AddActorLocalOffset(FVector(0, 0, -FallSpeed), true);
}


bool UPhysicsCalculator::OnGround() const
{
	AActor* Owner = GetOwner();
	if (!Owner) return false;

	FVector ActorLocation = Owner->GetActorLocation();
	FVector ActorScale = Owner->GetActorScale();
	FVector BoxExtent(40.0f * ActorScale.X, 20.0f * ActorScale.Y, 15.0f); // 薄い足元のボックス

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

//#if WITH_EDITOR
//	DrawDebugBox(
//		GetWorld(),
//		StartTrace,
//		BoxExtent,
//		ActorRotation, // ★ここも回転を反映
//		bHit ? FColor::Green : FColor::Red,
//		false, 1.0f
//	);
//#endif

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
const bool UPhysicsCalculator::HasLanded()
{
	return bHasJustLanded;
}
