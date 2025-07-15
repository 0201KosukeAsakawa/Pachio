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
#include "Kismet/GameplayStatics.h" 
#include "DataContainer/EffectMatchResult.h"
#include "Logic/Movement/PlayerMoveLogic.h"
#include "Manager/LevelManager.h"
#include "Manager/ColorManager.h"


// コンストラクタ
APlayerCharacter::APlayerCharacter()
{
	// 毎フレームTickを実行可能に設定
	PrimaryActorTick.bCanEverTick = true;
	// 各種コンポーネントを生成・初期化
	CameraComponent = CreateDefaultSubobject<UCameraHandlerComponent>(TEXT("CameraComponent"));
	MoveComp = CreateDefaultSubobject<UMoveComponent>(TEXT("MoveComponent"));
	AttackController = CreateDefaultSubobject<UAttackController>(TEXT("AttackController"));
	physics = CreateDefaultSubobject<UPhysicsCalculator>(TEXT("Physics"));
	colorController = CreateDefaultSubobject<UColorControllerComponent>(TEXT("ColorController"));
	InvincibilityComponent = CreateDefaultSubobject<UInvincibilityComponent>(TEXT("InvincibilityComponent"));
	// カメラコンポーネントの初期化（ルートコンポーネントを親に設定）
	CameraComponent->Init(RootComponent);

}

// ゲーム開始時の初期化処理
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	// 移動ロジック初期化
	InitMovementLogic();
	// ステート管理・攻撃管理初期化
	InitStateAndAttack();
	// 物理パラメータ設定
	InitPhysicsSettings();
	// 入力設定初期化
	InitInput();
	// 視覚関連設定（アウトラインなど）
	InitVisualSettings();
	GetCharacterMovement()->MaxWalkSpeed = MoveSpeed;
	// ColorManager に登録
	ALevelManager::GetInstance(GetWorld())->GetColorManager()->RegisterTarget(EColorTargetType::Responders, this);
	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (PC)
	{
		PC->bShowMouseCursor = false;
		PC->SetInputMode(FInputModeGameOnly());
	}
	GetCharacterMovement()->SetWalkableFloorAngle(60.f);
}

// 毎フレーム呼ばれる更新処理
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// ステートマネージャーが存在しない場合は処理中断
	if (!StateManager)
		return;
	Circle();
	// ステートマネージャーの時間経過更新処理を実行
	StateManager->Update(DeltaTime);

}

// プレイヤー入力バインド処理
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// UPlayerInputComponent から独自の入力バインディング処理を呼び出す
	UPlayerInputComponent* PlayerInputData = GetComponentByClass<UPlayerInputComponent>();
	if (PlayerInputData)
	{
		PlayerInputData->BindInput(PlayerInputComponent);
	}
}

// 現在のプレイヤーステートを取得
UPlayerStateComponent* APlayerCharacter::GetPlayerState() const
{
	return StateManager->GetCurrentState();
}

// ダメージ処理
bool APlayerCharacter::TakeDamage(FAttackData Data, float damage, const AActor*)
{
	// 無敵状態ならダメージを無効化
	if (InvincibilityComponent->IsInvincible())
		return false;

	if (!StateManager)
		return false;

	// ダメージを受けたら無敵時間開始
	InvincibilityComponent->StartInvincible();

	// 現在のステートにダメージ処理を委譲
	return StateManager->GetCurrentState()->TakeDamage();
}

void APlayerCharacter::ResetBuff()
{
	 JumpBuff = 1;
}

void APlayerCharacter::Circle()
{
	float DeltaX, DeltaY;
	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (!PC) return;

	PC->GetInputMouseDelta(DeltaX, DeltaY);
	FVector2D CurrentDir(DeltaX, DeltaY);

	if (CurrentDir.SizeSquared() > 4.0f)
	{
		CurrentDir.Normalize();

		if (bHasPrevMouse)
		{
			float CrossZ = PrevMouseDir.X * CurrentDir.Y - PrevMouseDir.Y * CurrentDir.X;

			if (CrossZ > 0)
			{
				ChangeColor(-0.01f);  // 左回し → -1
			}
			else if (CrossZ < 0)
			{
				ChangeColor(0.01f); // 右回し → +1
			}
		}

		PrevMouseDir = CurrentDir;
		bHasPrevMouse = true;
	}
}

// 移動入力処理（MoveCompを通して移動方向を取得し移動）
void APlayerCharacter::Movement(const FInputActionValue& Value)
{
	if (!MoveComp)
		return;

	// 移動方向をMoveCompのロジックから取得
	FVector direction = MoveComp->Movement(0, this, Value);
	// 速度は現在のステートが持つ移動速度を使用
	AddMovementInput(direction, MoveSpeed);

	// 移動方向がある場合はキャラクターの向きを滑らかに回転させる
	if (!direction.IsNearlyZero())
	{
		// 目標の回転角度を計算（キャラクター位置→移動方向のベクトル）
		FRotator TargetRot = UKismetMathLibrary::FindLookAtRotation(
			GetActorLocation(),
			GetActorLocation() + direction
		);

		// PitchとRollを0に固定（上下の傾きを防止）
		TargetRot.Pitch = 0.0f;
		TargetRot.Roll = 0.0f;

		// 現在の回転から目標回転へ一定速度で補間（スムーズな回転）
		FRotator SmoothRot = FMath::RInterpTo(
			GetActorRotation(),
			TargetRot,
			GetWorld()->GetDeltaSeconds(),
			10.0f
		);

		// キャラクターの回転を更新
		SetActorRotation(SmoothRot);
	}

	return;
}

// ジャンプ処理（地面に接地している場合のみ力を加える）
// 移動方向はMovement関数で取得済みのFVector directionをジャンプでも使いたいので
// Movement関数のdirectionをJump関数に渡すか、Jump関数内で再取得する必要あり

void APlayerCharacter::Jump(const FInputActionValue& Value)
{
	if (!physics || !physics->OnGround())
		return;

	// ジャンプ力を掛けて力を加える
	physics->AddForce(GetActorUpVector(), JumpForce);
}


// ダッシュ・スキル開始処理
void APlayerCharacter::Action(const FInputActionValue& Value)
{

}

// ダッシュ終了処理（速度を元に戻す）
void APlayerCharacter::StopAction()
{

}

void APlayerCharacter::OnMouseScroll(const FInputActionValue& Value)
{
	float ScrollValue = Value.Get<float>();

	if (ScrollValue > 0.1f)
	{
		ChangeColor(0.01);
	}
	else if (ScrollValue < -0.01f)
	{
		ChangeColor(-0.1);
	}
}

void APlayerCharacter::ChangeColor(float value)
{
	if (!colorController)
		return;

	colorController->AdjustColor(value);
}

// カラーモードを右にシフト（次の色モードへ変更）
void APlayerCharacter::ShiftArrayRightColorMode()
{
	if (!colorController)
		return;

	colorController->ChangeMode(1);
}

// カラーモードを左にシフト（前の色モードへ変更）
void APlayerCharacter::ShiftArrayLeftColorMode()
{
	if (!colorController)
		return;

	colorController->ChangeMode(-1);
}

// 状態の変更（ステートタグを指定して遷移）
bool APlayerCharacter::ChangeState(FString Tag)
{
	return StateManager->ChangeState(Tag);
}

// 移動ロジックの初期化（MoveCompにプレイヤー用移動ロジックをセット）
void APlayerCharacter::InitMovementLogic()
{
	UPlayerMoveLogic* PlayerLogic = NewObject<UPlayerMoveLogic>(this);
	MoveComp->Init(this, PlayerLogic);
}

// ステート管理・攻撃管理の初期化
void APlayerCharacter::InitStateAndAttack()
{
	// StateManager を指定のクラスで生成
	StateManager = NewObject<UStateManager>(this, StateManagerClass);

	if (!AttackController || !StateManagerClass)
		return;

	// 攻撃コントローラの初期化と攻撃登録
	AttackController->Init(GetWorld());
	AttackController->ResetMap();
	AttackController->RegisterAttackComponent("Stomp");
	//AttackController->RegisterAttackComponent("Upper");

	// ステートマネージャーのコンポーネント登録・初期化
	StateManager->RegisterComponent();
	StateManager->Init(this, GetWorld());
}

// 物理パラメータの初期化（摩擦・重力設定など）
void APlayerCharacter::InitPhysicsSettings()
{
	physics->RegisterComponent();
	// 重力を加える（値は任意、固定で10.0fを加算）
	physics->SetGravityScale(10.0f);

	auto* Move = GetCharacterMovement();

	// 摩擦や重力のパラメータ調整
	Move->BrakingFrictionFactor = 2.0f;
	Move->GroundFriction = 8.0f;
	Move->GravityScale = 0.0f; // 重力は自前のphysicsで制御しているため無効化
}

// 入力関連の初期化（コンポーネントのコントローラ参照を設定）
void APlayerCharacter::InitInput()
{
	UPlayerInputComponent* PlayerInputData = GetComponentByClass<UPlayerInputComponent>();
	if (PlayerInputData)
	{
		PlayerInputData->Init(Controller);
	}
}

// 視覚的設定（メッシュのアウトライン表示など）
void APlayerCharacter::InitVisualSettings()
{
	UStaticMeshComponent* pMesh = UFunctionLibrary::FindComponentByName<UStaticMeshComponent>(this, "StaticMesh");
	if (pMesh)
	{
		// カスタム深度レンダーを有効にしてアウトラインを表示
		pMesh->SetRenderCustomDepth(true);
		pMesh->SetCustomDepthStencilValue(10);
	}
}

void APlayerCharacter::ColorAction(FLinearColor Color)
{
	ApplyEffectFromColor(Color);
}

void APlayerCharacter::ApplyEffectFromColor(const FLinearColor& Color)
{
	// 色から最も近いバフ効果と強度を取得
	FEffectMatchResult Match = ALevelManager::GetInstance(GetWorld())
		->GetColorManager()
		->GetClosestEffectByHue(Color);
	ResetBuff();
	switch (Match.ClosestEffect)
	{
	case EBuffEffect::JumpBoost:
	{
		JumpBuff = 1.0f + 1.0f * Match.StrengthRatio;
		UE_LOG(LogTemp, Log, TEXT("[ColorEffect] JumpBoost applied: %.2f"), Match.StrengthRatio);
		break;
	}

	case EBuffEffect::SpeedBoost:
	{
		GetCharacterMovement()->MaxWalkSpeed = 1000.0f + 400.0f * Match.StrengthRatio;

		UE_LOG(LogTemp, Log, TEXT("[ColorEffect] SpeedBoost applied: %.2f"), Match.StrengthRatio);
		break;
	}

	case EBuffEffect::Shield:
	{
		// Shield効果が未実装
		UE_LOG(LogTemp, Warning, TEXT("[ColorEffect] Shield effect detected but not implemented."));
		break;
	}

	default:
	{
		UE_LOG(LogTemp, Warning, TEXT("[ColorEffect] No matching effect. Effect type unknown or invalid."));
		break;
	}
	}
}




	void APlayerCharacter::OnStickRotate(const FVector2D& StickInput)
	{
		const float DeadZone = 0.2f;
		if (StickInput.SizeSquared() < DeadZone)
			return;

		FVector2D InputDir = StickInput.GetSafeNormal(); // 正規化

		if (!bHasPrevInputDir)
		{
			PrevInputDir = InputDir;
			bHasPrevInputDir = true;
			return;
		}

		// 2Dクロス積で回転方向判定（Z成分だけ取る）
		float CrossZ = InputDir.X * PrevInputDir.Y - InputDir.Y * PrevInputDir.X;

		const float epsilon = 0.01f;
		if (CrossZ > epsilon)
		{
			UE_LOG(LogTemp, Log, TEXT("回転方向：左回り（反時計回り）"));
			ChangeColor(-0.1);
			PrevInputDir = InputDir;
		}
		else if (CrossZ < -epsilon)
		{
			UE_LOG(LogTemp, Log, TEXT("回転方向：右回り（時計回り）"));
			ChangeColor(0.1);
			PrevInputDir = InputDir;
		}

	}
void APlayerCharacter::OnStickMove(const FInputActionValue& Value)
{
	FVector2D StickInput = Value.Get<FVector2D>();
	OnStickRotate(StickInput);
}