// Copyright notice を Description ページで記載

// インクルード

#include "Player/PlayerCharacter.h"
#include "Player/State/PlayerDefaultState.h"
#include "Player/State/PlayerFireState.h"
#include "Player/State/StateManager.h"
#include "Components/PhysicsCalculator.h"
#include "Components/StaticMeshComponent.h"
#include "Components/AttackComponent.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/AttackController.h"
#include "Components/MoveComponent.h"
#include "Components/ColorControllerComponent.h"
#include "Components/PlayerInputComponent.h"
#include "Components/CameraHandlerComponent.h"
#include "Components/InvincibilityComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "FunctionLibrary.h"
#include "InputAction.h"
#include "Kismet/KismetMathLibrary.h"
#include "DataContainer/EffectMatchResult.h"
#include "Logic/Movement/PlayerMoveLogic.h"
#include "Manager/LevelManager.h"


// コンストラクタ
APlayerCharacter::APlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true; // Tickを有効化

	CameraComponent = CreateDefaultSubobject<UCameraHandlerComponent>(TEXT("CameraComponent"));
	MoveComp = CreateDefaultSubobject<UMoveComponent>(TEXT("MoveComponent"));
	AttackController = CreateDefaultSubobject<UAttackController>(TEXT("AttackController"));
	physics = CreateDefaultSubobject<UPhysicsCalculator>(TEXT("Physics"));
	colorController = CreateDefaultSubobject<UColorControllerComponent>(TEXT("ColorController"));
	InvincibilityComponent = CreateDefaultSubobject<UInvincibilityComponent>(TEXT("InvincibilityComponent"));

	CameraComponent->Init(RootComponent);
}

// ゲーム開始時に呼ばれる関数
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	UPlayerMoveLogic* PlayerLogic = NewObject<UPlayerMoveLogic>(this);
	MoveComp->Init(this, PlayerLogic);

	// 初期位置を保存
	PreviousLocation = GetActorLocation();

	bIsDashing = false; // 初期状態ではダッシュしていない

	// 攻撃コンポーネントの生成
	StateManager = NewObject<UStateManager>(this, StateManagerClass);

	physics->RegisterComponent();            // Tick対象になる

	if (!AttackController || !StateManagerClass)
		return;

	AttackController->Init(GetWorld());
	AttackController->ResetMap();
	AttackController->RegisterAttackComponent("Stomp");
	AttackController->RegisterAttackComponent("Upper");
	StateManager->RegisterComponent(); // Register as component
	StateManager->Init(this, GetWorld());

	//プレイヤーの座標を保存
	PlayerOldLocation = GetActorLocation();
	
	UPlayerInputComponent* PlayerInputData = GetComponentByClass<UPlayerInputComponent>();
	if (PlayerInputData)
	{
		PlayerInputData->Init(Controller);
	}

	GetCharacterMovement()->BrakingFrictionFactor = 2.0f; // 止まる速さを上げる
	GetCharacterMovement()->GroundFriction = 8.0f; // 地面との摩擦を強化
	// 重力スケールを強化（より素早い落下）
	GetCharacterMovement()->GravityScale = 0.0f;

	UStaticMeshComponent* pMesh = UFunctionLibrary::FindComponentByName<UStaticMeshComponent>(this, "StaticMesh");
	// プレイヤーメッシュにCustom Depthを有効化
	if (pMesh)
	{
		pMesh->SetRenderCustomDepth(true);
		pMesh->SetCustomDepthStencilValue(10);
	}
}

void APlayerCharacter::Tick(float DeltaTime)
{
	FVector temp = GetVelocity();

	Super::Tick(DeltaTime);

	temp = GetVelocity();

	//攻撃の判定が消えていたら即時リターン
	if (!StateManager)
		return;

	//ステートマネージャーの経過処理呼び出し
	StateManager->Update(DeltaTime);

	//プレイヤーのY座標取得
	float PlayerY = GetActorLocation().Y;


	//// 地上ならば攻撃判定を無効にする
	//if (physics->OnGround())
	//{
	//	UpperAttackBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//	StompAttackBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//}
	//// 空中ならば攻撃判定を有効にする
	//else
	//{
	//	// UpperAttackBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	//	StompAttackBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	//}

	//if (GetActorLocation().Z < PreviousLocation.Z)
	//{
	//	// 前のフレームよりも下に移動したときの処理
	//	UpperAttackBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//}
	//else
	//{
	//	UpperAttackBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	//}
	// 現在の位置を保存
	PreviousLocation.Z = GetActorLocation().Z;

	physics->AddGravity(10.0f);
	PlayerOldLocation = GetActorLocation();
}

// 入力のバインド
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	UPlayerInputComponent* PlayerInputData = GetComponentByClass<UPlayerInputComponent>();
	if (PlayerInputData)
	{
		PlayerInputData->BindInput(PlayerInputComponent);
	}
}

UPlayerStateComponent* APlayerCharacter::GetPlayerState() const
{
	return StateManager->GetCurrentState();
}

bool APlayerCharacter::TakeDamage(FAttackData Data, float damage , const AActor*)
{
	if (InvincibilityComponent->IsInvincible())
		return false; // 無敵状態の場合、ダメージを無視

	if (!StateManager)
		return false;

	// 無敵時間開始
	InvincibilityComponent->StartInvincible();

	return StateManager->GetCurrentState()->TakeDamage();
}

// 移動処理（StateManager 経由）
void APlayerCharacter::Movement(const FInputActionValue& Value)
{
	if (!MoveComp)
		return;

	FVector direction = (MoveComp->Movement(0, this, Value));
	//キャラクターを移動させる
	AddMovementInput(direction, StateManager->GetCurrentState()->GetMoveSpeed());

	// 入力がある場合のみ、キャラクターの向きを滑らかに回転させる
	if (!direction.IsNearlyZero())
	{
		// 向くべき方向を計算
		FRotator TargetRot = UKismetMathLibrary::FindLookAtRotation(
			GetActorLocation(),
			GetActorLocation() + direction
		);

		// Pitch（上下）、Roll（傾き）は固定
		TargetRot.Pitch = 0.0f;
		TargetRot.Roll = 0.0f;

		// 現在の回転と目標の回転の間をスムーズに補間
		FRotator SmoothRot = FMath::RInterpTo(
			GetActorRotation(),
			TargetRot,
			GetWorld()->GetDeltaSeconds(),
			10.0f // 補間スピード
		);

		// キャラクターの回転を設定
		SetActorRotation(SmoothRot);
	}

	return;
}

// ジャンプ処理（ジャンプ中に上攻撃の判定を有効化）
void APlayerCharacter::Jump(const FInputActionValue& Value)
{
	//ジャンプが可能な状態なら
	if (!physics->OnGround())
		return;

		physics->AddForce(GetActorUpVector(), 12);
}

// ジャンプ終了処理
void APlayerCharacter::JumpStop(const FInputActionValue& Value)
{
	ACharacter::StopJumping();
}

// ダッシュ開始（移動速度上昇）
void APlayerCharacter::Action(const FInputActionValue& Value)
{
	if (!bHasUsedSkill)
	{
		// StateManagerから現在のステートを取得
		UPlayerStateComponent* CurrentState = StateManager->GetCurrentState();
		if (CurrentState)
		{
			// UPlayerFireState にキャストできれば
			UPlayerFireState* FireState = Cast<UPlayerFireState>(CurrentState);
			if (FireState)
			{
				// 引数が未使用なので空の値を渡す
				FInputActionValue DummyValue;
				FireState->OnSkill(DummyValue);
				bHasUsedSkill = true;
			}
		}
	}
	//ダッシュ状態じゃなければ
	if (!bIsDashing)
	{
		//プレイヤーの速度を上昇させ、フラグをオンにする
		GetCharacterMovement()->MaxWalkSpeed = 1200.0f;
		bIsDashing = true;
	}
}

// ダッシュ終了（移動速度戻す）
void APlayerCharacter::StopAction()
{
	//ダッシュ状態なら
	if (bIsDashing)
	{
		//プレイヤーの速度を元に戻し、フラグをオフにする
		GetCharacterMovement()->MaxWalkSpeed = 600.0f;
		bIsDashing = false;
		bHasUsedSkill = false;
	}
}

void APlayerCharacter::DecreaseColor()
{
	if (!colorController)
		return;

	colorController->AdjustColor(0.001);
}

void APlayerCharacter::IncreaseColor()
{
	if (!colorController)
		return;

	colorController->AdjustColor(-0.001);
}

void APlayerCharacter::ShiftArrayRightColorMode()
{
	if (!colorController)
		return;

	colorController->ChangeMode(1);
}

void APlayerCharacter::ShiftArrayLeftColorMode()
{
	if (!colorController)
		return;

	colorController->ChangeMode(-1);
}


// 状態の変更（タグ指定）
bool APlayerCharacter::ChangeState(FString Tag)
{
	return StateManager->ChangeState(Tag);
}

void APlayerCharacter::ApplyEffectFromColor(const FLinearColor& Color)
{
	/*FEffectMatchResult Match = ALevel GetClosestEffectByHue(Color);

	switch (Match.ClosestEffect)
	{
	case EFilterEffectType::JumpBoost:
		GetCharacterMovement()->JumpZVelocity = 600.0f + 600.0f * Match.StrengthRatio;
		break;

	case EFilterEffectType::SpeedBoost:
		GetCharacterMovement()->MaxWalkSpeed = 600.0f + 400.0f * Match.StrengthRatio;
		break;

	case EFilterEffectType::Shield:
		ShieldStrength = BaseShield + (MaxShield - BaseShield) * Match.StrengthRatio;
		break;

	default:
		break;
	}*/
}
