#include "Objects/MovingObject.h"
#include "Sound/SoundManager.h"
#include "ColorUtilityLibrary.h"
#include "Components/BoxComponent.h"
#include "UE5Coro.h"

using namespace UE5Coro;

UMoveOnColorComponent::UMoveOnColorComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	FootTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("FootTrigger"));
	FootTrigger->SetupAttachment(this);
	FootTrigger->SetGenerateOverlapEvents(true);
	FootTrigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	FootTrigger->SetCollisionResponseToAllChannels(ECR_Ignore);
	FootTrigger->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

void UMoveOnColorComponent::BeginPlay()
{
	Super::BeginPlay();

	FootTrigger->OnComponentBeginOverlap.AddDynamic(this, &UMoveOnColorComponent::OnFootBeginOverlap);
	FootTrigger->OnComponentEndOverlap.AddDynamic(this, &UMoveOnColorComponent::OnFootEndOverlap);

	UE_LOG(LogTemp, Warning, TEXT("BeginPlay - MovementMode: %d, bAutoLoop: %d"),
		(int32)MovementMode, bAutoLoop);

	// Shuttleモードかつ自動ループの場合のみ自動開始
	if (MovementMode == EColorMovementMode::Shuttle && bAutoLoop)
	{
		UE_LOG(LogTemp, Warning, TEXT("Starting AutoLoop from BeginPlay (Shuttle mode)"));
		AutoLoopMovement();
	}
}

void UMoveOnColorComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	bIsAutoLoopRunning = false;
	Super::EndPlay(EndPlayReason);
}

void UMoveOnColorComponent::Initialize()
{
	UObjectColorComponent::Initialize();

	InitialWorldLocation = GetOwner()->GetActorLocation();

	// 地点Aのワールド座標を計算
	if (LocationAMode == ELocationMode::Local)
	{
		WorldLocationA = InitialWorldLocation + LocationA;
		UE_LOG(LogTemp, Log, TEXT("Location A (Local): %s -> World: %s"),
			*LocationA.ToString(), *WorldLocationA.ToString());
	}
	else
	{
		WorldLocationA = LocationA;
		UE_LOG(LogTemp, Log, TEXT("Location A (World): %s"), *WorldLocationA.ToString());
	}

	// 地点Bのワールド座標を計算
	if (LocationBMode == ELocationMode::Local)
	{
		WorldLocationB = InitialWorldLocation + LocationB;
		UE_LOG(LogTemp, Log, TEXT("Location B (Local): %s -> World: %s"),
			*LocationB.ToString(), *WorldLocationB.ToString());
	}
	else
	{
		WorldLocationB = LocationB;
		UE_LOG(LogTemp, Log, TEXT("Location B (World): %s"), *WorldLocationB.ToString());
	}

	// 初期位置を地点Aに設定
	GetOwner()->SetActorLocation(WorldLocationA);
	bCurrentTargetIsB = false;
}

void UMoveOnColorComponent::ActivateDirect(const FLinearColor& InColor)
{
	switch (MovementMode)
	{
	case EColorMovementMode::Toggle:
		HandleToggleMode(InColor);
		break;

	case EColorMovementMode::Shuttle:
		HandleShuttleMode(InColor);
		break;
	}
}

void UMoveOnColorComponent::HandleToggleMode(const FLinearColor& InColor)
{
	// 色が一致する場合
	if (UColorUtilityLibrary::IsHueSimilar(InColor, CurrentColor))
	{
		// 自動ループが有効な場合は開始
		if (bAutoLoop && !bIsAutoLoopRunning)
		{
			UE_LOG(LogTemp, Warning, TEXT("Toggle: Starting AutoLoop (color match)"));
			AutoLoopMovement();
		}
		else if (!bAutoLoop)
		{
			// 自動ループ無効時は通常の移動
			MoveWithEasingAsync(WorldLocationB, MoveDuration);
		}
	}
	else
	{
		// 色が不一致の場合

		// 自動ループ実行中なら停止
		if (bIsAutoLoopRunning)
		{
			UE_LOG(LogTemp, Warning, TEXT("Toggle: Stopping AutoLoop (color mismatch)"));
			bIsAutoLoopRunning = false;
		}

		// Location Aに移動
		MoveWithEasingAsync(WorldLocationA, MoveDuration);
	}
}

void UMoveOnColorComponent::HandleShuttleMode(const FLinearColor& InColor)
{
	// 自動ループ中は色変化による移動をしない
	if (bAutoLoop)
	{
		return;
	}

	FVector TargetLocation;

	if (bReverseOnColorChange)
	{
		// 現在の目標地点を反転
		bCurrentTargetIsB = !bCurrentTargetIsB;
		TargetLocation = bCurrentTargetIsB ? WorldLocationB : WorldLocationA;
	}
	else
	{
		// 色が一致する場合は次の地点へ
		if (UColorUtilityLibrary::IsHueSimilar(InColor, CurrentColor))
		{
			bCurrentTargetIsB = !bCurrentTargetIsB;
			TargetLocation = bCurrentTargetIsB ? WorldLocationB : WorldLocationA;
		}
		else
		{
			// 色が一致しない場合は現在の目標を維持
			return;
		}
	}

	// 新しい移動を開始
	MoveWithEasingAsync(TargetLocation, MoveDuration);
}

UE5Coro::TCoroutine<> UMoveOnColorComponent::AutoLoopMovement()
{
	// 既に実行中なら何もしない
	if (bIsAutoLoopRunning.exchange(true))
	{
		UE_LOG(LogTemp, Warning, TEXT("AutoLoop: Already running, skipping"));
		co_return;
	}

	UE_LOG(LogTemp, Warning, TEXT("AutoLoop: Started - LocationA: %s, LocationB: %s"),
		*WorldLocationA.ToString(), *WorldLocationB.ToString());

	// 初期位置は地点A想定なので、最初は地点Bへ向かう
	bCurrentTargetIsB = true;

	int32 LoopCount = 0;

	while (bIsAutoLoopRunning)
	{
		LoopCount++;
		UE_LOG(LogTemp, Warning, TEXT("AutoLoop: Loop iteration %d"), LoopCount);

		// オーナーチェック
		if (!GetOwner())
		{
			UE_LOG(LogTemp, Error, TEXT("AutoLoop: Owner is null, stopping"));
			break;
		}

		// 次の目標地点を決定
		FVector TargetLocation = bCurrentTargetIsB ? WorldLocationB : WorldLocationA;

		UE_LOG(LogTemp, Warning, TEXT("AutoLoop: Moving to %s (from %s)"),
			bCurrentTargetIsB ? TEXT("Location B") : TEXT("Location A"),
			*GetOwner()->GetActorLocation().ToString());

		// 移動開始時刻を記録
		const FVector StartLocation = GetOwner()->GetActorLocation();
		float ElapsedTime = 0.0f;

		// 目標地点と現在地が同じ場合のチェック
		if (FVector::Dist(StartLocation, TargetLocation) < 1.0f)
		{
			UE_LOG(LogTemp, Warning, TEXT("AutoLoop: Already at target location, skipping movement"));
		}
		else
		{
			// 移動ループ
			int32 FrameCount = 0;
			while (ElapsedTime < MoveDuration)
			{
				co_await Latent::NextTick();
				FrameCount++;

				if (!GetOwner() || !bIsAutoLoopRunning)
				{
					UE_LOG(LogTemp, Error, TEXT("AutoLoop: Interrupted during movement"));
					co_return;
				}

				ElapsedTime += GetWorld()->GetDeltaSeconds();
				float Alpha = FMath::Clamp(ElapsedTime / MoveDuration, 0.0f, 1.0f);

				// イージング関数を適用
				Alpha = ApplyEasing(Alpha);

				const FVector CurrentLocation = GetOwner()->GetActorLocation();
				const FVector NewLocation = FMath::Lerp(StartLocation, TargetLocation, Alpha);
				const FVector DeltaMove = NewLocation - CurrentLocation;

				GetOwner()->SetActorLocation(NewLocation);

				// 無効なアクターを削除
				AttachedActors.RemoveAll([](const TWeakObjectPtr<AActor>& Actor)
					{
						return !Actor.IsValid();
					});

				// 乗っているアクターを移動
				for (const TWeakObjectPtr<AActor>& ActorPtr : AttachedActors)
				{
					if (AActor* Actor = ActorPtr.Get())
					{
						Actor->AddActorWorldOffset(DeltaMove, true);
					}
				}

				// 子アクターを移動
				for (AActor* ChildActor : Child)
				{
					if (ChildActor)
					{
						ChildActor->AddActorWorldOffset(DeltaMove, true);
					}
				}
			}

			UE_LOG(LogTemp, Log, TEXT("AutoLoop: Movement completed in %d frames"), FrameCount);
		}

		// 最終位置を確実に設定
		if (GetOwner())
		{
			GetOwner()->SetActorLocation(TargetLocation);
		}

		if (!GetOwner() || !bIsAutoLoopRunning)
		{
			UE_LOG(LogTemp, Warning, TEXT("AutoLoop: Stopping before wait"));
			break;
		}

		// 到着地点での待機時間を取得
		float WaitTime = bCurrentTargetIsB ? WaitTimeAtB : WaitTimeAtA;

		UE_LOG(LogTemp, Warning, TEXT("AutoLoop: Arrived at %s, waiting %.2f seconds"),
			bCurrentTargetIsB ? TEXT("Location B") : TEXT("Location A"), WaitTime);

		// 待機
		if (WaitTime > 0.0f)
		{
			co_await Latent::Seconds(WaitTime);
		}
		else
		{
			// 待機時間が0でも1フレームは待つ
			co_await Latent::NextTick();
		}

		UE_LOG(LogTemp, Log, TEXT("AutoLoop: Wait completed"));

		// キャンセルチェック（待機後）
		if (!GetOwner() || !bIsAutoLoopRunning)
		{
			UE_LOG(LogTemp, Warning, TEXT("AutoLoop: Stopping after wait"));
			break;
		}

		// 目標を反転
		bCurrentTargetIsB = !bCurrentTargetIsB;
		UE_LOG(LogTemp, Log, TEXT("AutoLoop: Target reversed, next target is %s"),
			bCurrentTargetIsB ? TEXT("Location B") : TEXT("Location A"));
	}

	UE_LOG(LogTemp, Warning, TEXT("AutoLoop: Stopped after %d iterations"), LoopCount);
	bIsAutoLoopRunning = false;
}

float UMoveOnColorComponent::ApplyEasing(float Alpha) const
{
	switch (EasingType)
	{
	case EMovementEasing::Linear:
		return Alpha;

	case EMovementEasing::EaseIn:
		return Alpha * Alpha;

	case EMovementEasing::EaseOut:
		return 1.0f - FMath::Square(1.0f - Alpha);

	case EMovementEasing::EaseInOut:
		if (Alpha < 0.5f)
		{
			return 2.0f * Alpha * Alpha;
		}
		else
		{
			const float Temp = -2.0f * Alpha + 2.0f;
			return 1.0f - (Temp * Temp) / 2.0f;
		}

	case EMovementEasing::SmoothStep:
		return Alpha * Alpha * (3.0f - 2.0f * Alpha);

	default:
		return Alpha;
	}
}

TCoroutine<> UMoveOnColorComponent::MoveWithEasingAsync(FVector Target, float Duration)
{
	// 自動ループ中は手動移動を無視
	if (bIsAutoLoopRunning)
	{
		UE_LOG(LogTemp, Warning, TEXT("Manual move ignored - AutoLoop is running"));
		co_return;
	}

	if (!GetOwner() || Duration < MIN_DURATION)
	{
		if (GetOwner())
		{
			GetOwner()->SetActorLocation(Target);
		}
		co_return;
	}

	const FVector StartLocation = GetOwner()->GetActorLocation();
	float ElapsedTime = 0.0f;

	while (ElapsedTime < Duration)
	{
		co_await Latent::NextTick();

		// オーナーが破棄された場合
		if (!GetOwner())
		{
			co_return;
		}

		ElapsedTime += GetWorld()->GetDeltaSeconds();
		float Alpha = FMath::Clamp(ElapsedTime / Duration, 0.0f, 1.0f);

		// イージング関数を適用
		Alpha = ApplyEasing(Alpha);

		const FVector CurrentLocation = GetOwner()->GetActorLocation();
		const FVector NewLocation = FMath::Lerp(StartLocation, Target, Alpha);
		const FVector DeltaMove = NewLocation - CurrentLocation;

		GetOwner()->SetActorLocation(NewLocation);

		// 無効なアクターを削除
		AttachedActors.RemoveAll([](const TWeakObjectPtr<AActor>& Actor)
			{
				return !Actor.IsValid();
			});

		// 乗っているアクターを移動
		for (const TWeakObjectPtr<AActor>& ActorPtr : AttachedActors)
		{
			if (AActor* Actor = ActorPtr.Get())
			{
				Actor->AddActorWorldOffset(DeltaMove, true);
			}
		}

		// 子アクターを移動
		for (AActor* ChildActor : Child)
		{
			if (ChildActor)
			{
				ChildActor->AddActorWorldOffset(DeltaMove, true);
			}
		}
	}

	// 最終位置を確実に設定
	if (GetOwner())
	{
		GetOwner()->SetActorLocation(Target);
	}
}

void UMoveOnColorComponent::OnFootBeginOverlap(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (!OtherActor || OtherActor == GetOwner())
	{
		return;
	}

	if (OtherComp && OtherComp->ComponentHasTag(TEXT("Interaction")))
	{
		return;
	}

	TWeakObjectPtr<AActor> ActorPtr(OtherActor);
	if (!AttachedActors.Contains(ActorPtr))
	{
		AttachedActors.Add(ActorPtr);
		UE_LOG(LogTemp, Log, TEXT("Added actor on top: %s"), *OtherActor->GetName());
	}
}

void UMoveOnColorComponent::OnFootEndOverlap(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	if (!OtherActor)
	{
		return;
	}

	if (OtherComp && OtherComp->ComponentHasTag(TEXT("Interaction")))
	{
		return;
	}

	TWeakObjectPtr<AActor> ActorPtr(OtherActor);
	if (AttachedActors.Remove(ActorPtr) > 0)
	{
		UE_LOG(LogTemp, Log, TEXT("Removed actor from top: %s"), *OtherActor->GetName());
	}
}