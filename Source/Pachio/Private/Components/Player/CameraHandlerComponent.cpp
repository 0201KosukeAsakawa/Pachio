#include "Components/CameraHandlerComponent.h"
#include "Camera/CameraComponent.h"
#include "UE5Coro.h"
#include "UE5Coro/Coroutine.h"

#include "Kismet/GameplayStatics.h"
#include "GameFramework/SpringArmComponent.h"


// コンストラクタ: 初期化
UCameraHandlerComponent::UCameraHandlerComponent()
{
	PrimaryComponentTick.bCanEverTick = true; // Tick有効

	// カメラコンポーネント生成
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));

	// 補間スピード初期値
	InterpSpeed = 3.0f;
}

// 初期化: カメラの位置を設定
void UCameraHandlerComponent::Init(TObjectPtr<USceneComponent> RootComponent)
{
	if (Camera == nullptr)
		return;

	// カメラをルートにアタッチ
	if (RootComponent)
	{
		Camera->SetupAttachment(RootComponent);
	}

	// 初期位置を設定
	SetCameraLocation(CameraViewType);

	// 初期FOVと回転を保存
	if (Camera)
	{
		OriginalFOV = Camera->FieldOfView;
		PreEventCameraRotation = Camera->GetComponentRotation();
	}
}

// Tick関数: 毎フレームカメラ位置を更新
void UCameraHandlerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// イベント演出中は通常の追従処理をスキップ
	if (!bIsInEventMode)
	{
		UpdateCameraPosition(DeltaTime);
	}
}

// カメラ追従処理
void UCameraHandlerComponent::UpdateCameraPosition(float DeltaTime)
{
	if (!Camera || !GetOwner())
		return;

	FVector PlayerLocation = GetOwner()->GetActorLocation();

	switch (CameraViewType)
	{
	case ECameraViewType::CharacterView:
	{
		// キャラクタービュー: プレイヤー前方＋少し上にカメラを配置
		FVector PlayerForward = GetOwner()->GetActorForwardVector();
		FVector AdjustedLocation = PlayerLocation + PlayerForward * 50.f;
		AdjustedLocation.Z += 50.f;

		// カメラの基準位置
		FVector CameraBaseLocation(-CameraDistance, AdjustedLocation.Y, AdjustedLocation.Z);

		// プレイヤーからの偏差
		FVector PlayerOffset(
			0.f,
			AdjustedLocation.Y - CameraBaseLocation.Y,
			AdjustedLocation.Z - CameraBaseLocation.Z
		);

		// ゆるく追従 (20%だけ追従)
		TargetCameraLocation = CameraBaseLocation + PlayerOffset * 0.2f;
		break;
	}
	case ECameraViewType::GridView:
	{
		// グリッドビュー: プレイヤーの現在グリッドを計算（YZ平面）
		FIntPoint NewGrid(
			FMath::FloorToInt(PlayerLocation.Y / GridSize.X),
			FMath::FloorToInt(PlayerLocation.Z / GridSize.Y)
		);

		// グリッドが変わったら更新
		if (NewGrid != CurrentGrid)
		{
			CurrentGrid = NewGrid;
		}

		// グリッドの中心位置を計算
		FVector GridCenter(
			-CameraDistance,
			CurrentGrid.X * GridSize.X + GridSize.X / 2.f,
			CurrentGrid.Y * GridSize.Y + GridSize.Y / 2.f
		);

		// プレイヤーのグリッド内偏差
		FVector PlayerOffset(
			0.f,
			PlayerLocation.Y - GridCenter.Y,
			PlayerLocation.Z - GridCenter.Z
		);

		// ゆるく追従 (20%だけ追従)
		TargetCameraLocation = GridCenter + PlayerOffset * 0.2f;
		break;
	}
	default:
		break;
	}

	// 現在位置から補間して滑らかに移動
	FVector CurrentLocation = Camera->GetComponentLocation();
	FVector NewLocation = FMath::VInterpTo(CurrentLocation, TargetCameraLocation, DeltaTime, InterpSpeed);
	Camera->SetWorldLocation(NewLocation);
}

// カメラ位置を直接セット（初期化や切替時）
void UCameraHandlerComponent::SetCameraLocation(ECameraViewType Type)
{
	if (!GetOwner())
		return;

	FVector PlayerLocation = GetOwner()->GetActorLocation();

	// グリッドの初期位置計算
	CurrentGrid = FIntPoint(
		FMath::FloorToInt(PlayerLocation.Y / GridSize.X),
		FMath::FloorToInt(PlayerLocation.Z / GridSize.Y)
	);

	// ビュータイプに応じた初期位置を計算
	switch (Type)
	{
	case ECameraViewType::CharacterView:
	{
		FVector PlayerForward = GetOwner()->GetActorForwardVector();
		FVector AdjustedLocation = PlayerLocation + PlayerForward * 50.f;
		AdjustedLocation.Z += 50.f;
		TargetCameraLocation = FVector(-CameraDistance, AdjustedLocation.Y, AdjustedLocation.Z);
		break;
	}
	case ECameraViewType::GridView:
	{
		TargetCameraLocation = FVector(
			-CameraDistance,
			CurrentGrid.X * GridSize.X + GridSize.X / 2.f,
			CurrentGrid.Y * GridSize.Y + GridSize.Y / 2.f
		);
		break;
	}
	}

	if (Camera)
	{
		Camera->SetWorldLocation(TargetCameraLocation);
	}
}

// カメラ設定を適用（グリッドサイズとオフセット）
void UCameraHandlerComponent::ApplyCameraSettings(FVector2D NewSize, float NewCameraDistance)
{
	GridSize = NewSize;
	CameraDistance = NewCameraDistance;

	if (!GetOwner())
		return;

	FVector PlayerLocation = GetOwner()->GetActorLocation();

	// 現在グリッドを更新
	CurrentGrid = FIntPoint(
		FMath::FloorToInt(PlayerLocation.Y / GridSize.X),
		FMath::FloorToInt(PlayerLocation.Z / GridSize.Y)
	);

	// カメラ位置をグリッド中央で計算
	TargetCameraLocation = FVector(
		-CameraDistance,
		CurrentGrid.X * GridSize.X + GridSize.X / 2.f,
		CurrentGrid.Y * GridSize.Y + GridSize.Y / 2.f
	);

	if (Camera)
	{
		Camera->SetWorldLocation(TargetCameraLocation);
	}
}

// カメラ設定を適用（グリッドサイズ・オフセット・ビュータイプ）
void UCameraHandlerComponent::ApplyCameraSettings(FVector2D NewSize, float NewCameraDistance, ECameraViewType NewViewType)
{
	CameraViewType = NewViewType;
	GridSize = NewSize;
	CameraDistance = NewCameraDistance;

	SetCameraLocation(CameraViewType);
}

// 現在のカメラ設定と一致しているか確認
bool UCameraHandlerComponent::IsParameterMatch(FVector2D NewSize, float NewCameraDistance, ECameraViewType NewType)
{
	return (GridSize == NewSize && CameraDistance == NewCameraDistance && CameraViewType == NewType);
}

// ========== イベント演出用コルーチン実装 ==========

// 基本的なフォーカス
UE5Coro::TCoroutine<> UCameraHandlerComponent::FocusOnLocation(
	FVector EventLocation,
	float FocusDuration,
	float MoveSpeed,
	bool bWaitForComplete,
	bool bLookAtTarget)
{
	using namespace UE5Coro;

	if (!Camera || !GetOwner())
		co_return;

	// イベントモード開始
	bIsInEventMode = true;
	PreEventTargetLocation = TargetCameraLocation;
	PreEventCameraRotation = Camera->GetComponentRotation();

	// イベント位置を設定（X座標は固定）
	FVector TargetLocation = EventLocation;
	TargetLocation.X = -CameraDistance;

	// ターゲットへの回転を計算
	FRotator TargetRotation = PreEventCameraRotation;
	if (bLookAtTarget)
	{
		FVector LookDirection = (EventLocation - TargetLocation).GetSafeNormal();
		TargetRotation = LookDirection.Rotation();
	}

	// カメラを目標位置まで移動
	if (bWaitForComplete)
	{
		// 移動完了まで待つ
		while (FVector::Dist(Camera->GetComponentLocation(), TargetLocation) > 50.0f ||
			(bLookAtTarget && !Camera->GetComponentRotation().Equals(TargetRotation, 1.0f)))
		{
			FVector CurrentLocation = Camera->GetComponentLocation();
			FVector NewLocation = FMath::VInterpTo(
				CurrentLocation,
				TargetLocation,
				GetWorld()->GetDeltaSeconds(),
				MoveSpeed
			);
			Camera->SetWorldLocation(NewLocation);

			// カメラの向きも補間
			if (bLookAtTarget)
			{
				FRotator CurrentRotation = Camera->GetComponentRotation();
				FRotator NewRotation = FMath::RInterpTo(
					CurrentRotation,
					TargetRotation,
					GetWorld()->GetDeltaSeconds(),
					MoveSpeed
				);
				Camera->SetWorldRotation(NewRotation);
			}

			co_await Latent::NextTick();
		}
	}
	else
	{
		// 即座に設定
		Camera->SetWorldLocation(TargetLocation);
		if (bLookAtTarget)
		{
			Camera->SetWorldRotation(TargetRotation);
		}
	}

	// 指定時間待機
	co_await Latent::Seconds(FocusDuration);

	// プレイヤー位置に戻る
	co_await ReturnToPlayer(MoveSpeed);

	// 元の回転に戻す
	if (bLookAtTarget)
	{
		float ElapsedTime = 0.0f;
		float ReturnDuration = 1.0f / MoveSpeed;

		while (ElapsedTime < ReturnDuration)
		{
			ElapsedTime += GetWorld()->GetDeltaSeconds();
			float Alpha = FMath::Clamp(ElapsedTime / ReturnDuration, 0.0f, 1.0f);

			FRotator CurrentRotation = Camera->GetComponentRotation();
			FRotator NewRotation = FMath::RInterpTo(
				CurrentRotation,
				PreEventCameraRotation,
				GetWorld()->GetDeltaSeconds(),
				MoveSpeed
			);
			Camera->SetWorldRotation(NewRotation);

			co_await Latent::NextTick();
		}

		Camera->SetWorldRotation(PreEventCameraRotation);
	}

	bIsInEventMode = false;
}

// 複数地点フォーカス
UE5Coro::TCoroutine<> UCameraHandlerComponent::FocusOnMultipleLocations(
	TArray<FVector> Locations,
	float DurationPerLocation,
	float MoveSpeed,
	bool bLookAtTarget)
{
	using namespace UE5Coro;

	if (!Camera || Locations.Num() == 0)
		co_return;

	bIsInEventMode = true;
	PreEventTargetLocation = TargetCameraLocation;
	PreEventCameraRotation = Camera->GetComponentRotation();

	for (const FVector& Location : Locations)
	{
		FVector TargetLocation = Location;
		TargetLocation.X = -CameraDistance;

		// ターゲットへの回転を計算
		FRotator TargetRotation = PreEventCameraRotation;
		if (bLookAtTarget)
		{
			FVector LookDirection = (Location - TargetLocation).GetSafeNormal();
			TargetRotation = LookDirection.Rotation();
		}

		// 各地点まで移動
		while (FVector::Dist(Camera->GetComponentLocation(), TargetLocation) > 50.0f ||
			(bLookAtTarget && !Camera->GetComponentRotation().Equals(TargetRotation, 1.0f)))
		{
			FVector CurrentLocation = Camera->GetComponentLocation();
			FVector NewLocation = FMath::VInterpTo(
				CurrentLocation,
				TargetLocation,
				GetWorld()->GetDeltaSeconds(),
				MoveSpeed
			);
			Camera->SetWorldLocation(NewLocation);

			// カメラの向きも補間
			if (bLookAtTarget)
			{
				FRotator CurrentRotation = Camera->GetComponentRotation();
				FRotator NewRotation = FMath::RInterpTo(
					CurrentRotation,
					TargetRotation,
					GetWorld()->GetDeltaSeconds(),
					MoveSpeed
				);
				Camera->SetWorldRotation(NewRotation);
			}

			co_await Latent::NextTick();
		}

		// その地点で待機
		co_await Latent::Seconds(DurationPerLocation);
	}

	// プレイヤー位置に戻る
	co_await ReturnToPlayer(MoveSpeed);

	// 元の回転に戻す
	if (bLookAtTarget)
	{
		float ElapsedTime = 0.0f;
		float ReturnDuration = 1.0f / MoveSpeed;

		while (ElapsedTime < ReturnDuration)
		{
			ElapsedTime += GetWorld()->GetDeltaSeconds();
			float Alpha = FMath::Clamp(ElapsedTime / ReturnDuration, 0.0f, 1.0f);

			FRotator CurrentRotation = Camera->GetComponentRotation();
			FRotator NewRotation = FMath::RInterpTo(
				CurrentRotation,
				PreEventCameraRotation,
				GetWorld()->GetDeltaSeconds(),
				MoveSpeed
			);
			Camera->SetWorldRotation(NewRotation);

			co_await Latent::NextTick();
		}

		Camera->SetWorldRotation(PreEventCameraRotation);
	}

	bIsInEventMode = false;
}

// プレイヤーに戻る
UE5Coro::TCoroutine<> UCameraHandlerComponent::ReturnToPlayer(float MoveSpeed)
{
	using namespace UE5Coro;

	if (!Camera || !GetOwner())
		co_return;

	// プレイヤー位置を再計算
	FVector PlayerLocation = GetOwner()->GetActorLocation();

	// 通常追従位置を計算(CameraViewTypeに応じて)
	FVector ReturnTarget;

	switch (CameraViewType)
	{
	case ECameraViewType::CharacterView:
	{
		FVector PlayerForward = GetOwner()->GetActorForwardVector();
		FVector AdjustedLocation = PlayerLocation + PlayerForward * 50.f;
		AdjustedLocation.Z += 50.f;
		ReturnTarget = FVector(-CameraDistance, AdjustedLocation.Y, AdjustedLocation.Z);
		break;
	}
	case ECameraViewType::GridView:
	{
		FIntPoint Grid(
			FMath::FloorToInt(PlayerLocation.Y / GridSize.X),
			FMath::FloorToInt(PlayerLocation.Z / GridSize.Y)
		);
		ReturnTarget = FVector(
			-CameraDistance,
			Grid.X * GridSize.X + GridSize.X / 2.f,
			Grid.Y * GridSize.Y + GridSize.Y / 2.f
		);
		break;
	}
	}

	// プレイヤー位置まで戻る
	while (FVector::Dist(Camera->GetComponentLocation(), ReturnTarget) > 100.0f)
	{
		FVector CurrentLocation = Camera->GetComponentLocation();
		FVector NewLocation = FMath::VInterpTo(
			CurrentLocation,
			ReturnTarget,
			GetWorld()->GetDeltaSeconds(),
			MoveSpeed
		);
		Camera->SetWorldLocation(NewLocation);

		co_await Latent::NextTick();
	}
}

// ズーム演出
UE5Coro::TCoroutine<> UCameraHandlerComponent::ZoomCamera(float TargetFOV, float Duration)
{
	using namespace UE5Coro;

	if (!Camera)
		co_return;

	float StartFOV = Camera->FieldOfView;
	float ElapsedTime = 0.0f;

	// ズームイン
	while (ElapsedTime < Duration)
	{
		ElapsedTime += GetWorld()->GetDeltaSeconds();
		float Alpha = FMath::Clamp(ElapsedTime / Duration, 0.0f, 1.0f);
		float NewFOV = FMath::Lerp(StartFOV, TargetFOV, Alpha);
		Camera->SetFieldOfView(NewFOV);

		co_await Latent::NextTick();
	}

	// 少し待機
	co_await Latent::Seconds(0.5f);

	// ズームアウト
	ElapsedTime = 0.0f;
	while (ElapsedTime < Duration)
	{
		ElapsedTime += GetWorld()->GetDeltaSeconds();
		float Alpha = FMath::Clamp(ElapsedTime / Duration, 0.0f, 1.0f);
		float NewFOV = FMath::Lerp(TargetFOV, OriginalFOV, Alpha);
		Camera->SetFieldOfView(NewFOV);

		co_await Latent::NextTick();
	}

	Camera->SetFieldOfView(OriginalFOV);
}

// カメラシェイク
UE5Coro::TCoroutine<> UCameraHandlerComponent::ShakeCamera(float Intensity, float Duration)
{
	using namespace UE5Coro;

	if (!Camera)
		co_return;

	FVector OriginalLocation = Camera->GetComponentLocation();
	float ElapsedTime = 0.0f;

	while (ElapsedTime < Duration)
	{
		ElapsedTime += GetWorld()->GetDeltaSeconds();

		// ランダムなオフセット（X軸は固定なのでYZのみ）
		FVector RandomOffset(
			0.0f,
			FMath::FRandRange(-Intensity, Intensity),
			FMath::FRandRange(-Intensity, Intensity)
		);

		// 減衰させる（時間経過で揺れを小さく）
		float DecayFactor = 1.0f - (ElapsedTime / Duration);
		RandomOffset *= DecayFactor;

		Camera->SetWorldLocation(OriginalLocation + RandomOffset);

		co_await Latent::NextTick();
	}

	// 元に戻す
	Camera->SetWorldLocation(OriginalLocation);
}