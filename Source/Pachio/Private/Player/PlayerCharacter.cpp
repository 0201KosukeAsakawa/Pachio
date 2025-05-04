// Copyright notice を Description ページで記載

// インクルード
#include "Player/PlayerCharacter.h"
#include "Player/State/PlayerDefaultState.h"
#include "Player/State/StateManager.h"
#include "Components/StaticMeshComponent.h"
#include "Components/AttackComponent.h"
#include "Components/BoxComponent.h"
#include "Components/AttackManagerComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "FunctionLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "InputAction.h"


// コンストラクタ
APlayerCharacter::APlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true; // Tickを有効化

	// SpringArmの作成とルートへのアタッチ
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);

	// Cameraの作成とSpringArmへのアタッチ
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);

	// SpringArmの初期設定
	SpringArm->TargetArmLength = 500.0f;
	SpringArm->SocketOffset = FVector(0.0f, 100.0f, 50.0f);
	SpringArm->bUsePawnControlRotation = false; // プレイヤー回転と連動しない
}

// ゲーム開始時に呼ばれる関数
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	bIsDashing = false; // 初期状態ではダッシュしていない

	// 攻撃コンポーネントの生成
	AttackManager = NewObject<UAttackManagerComponent>(this);

	if (AttackManager)
	{
		AttackManager->Init(GetWorld());
		AttackManager->ResetMap();
		AttackManager->RegisterAttackComponent("Stomp");
	}

	// ステートマネージャーの初期化
	StateManager = NewObject<UStateManager>();
	StateManager->Init(this, GetWorld());

	// 攻撃判定用のボックスコンポーネントを探す
	UpperAttackBox = UFunctionLibrary::FindComponentByName<UBoxComponent>(this, "A");
	StompAttackBox = UFunctionLibrary::FindComponentByName<UBoxComponent>(this, "B");

	// ボックスが見つからなければ処理中断
	if (!UpperAttackBox || !StompAttackBox)
		return;

	// 当たり判定のイベント登録
	UpperAttackBox->OnComponentBeginOverlap.AddDynamic(this, &APlayerCharacter::OnUpperAttack);
	StompAttackBox->OnComponentBeginOverlap.AddDynamic(this, &APlayerCharacter::OnStompAttack);

	// 初期状態では当たり判定を無効にする
	UpperAttackBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	StompAttackBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// SpringArm の回転継承設定
	if (SpringArm)
	{
		SpringArm->bInheritYaw = true;    // Yaw（左右）は継承する
		SpringArm->bInheritPitch = false; // Pitch（上下）は継承しない
		SpringArm->bInheritRoll = false;  // Roll（傾き）は継承しない
	}

	// カメラとプレイヤーの初期位置記録
	NewCameraLocation = Camera->GetComponentLocation();
	PlayerOldLocation = GetActorLocation();

	// 入力マッピングコンテキストの追加
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	// 重力スケールを強化（より素早い落下）
	GetCharacterMovement()->GravityScale = 3.0f;
}

// 毎フレーム呼ばれる関数
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//// カメラの位置をプレイヤーの移動に合わせて補正
	//if (SpringArm && Camera)
	//{
	//	// 現在位置との差分でカメラをY方向のみ追従
	//	FVector deff = GetActorLocation() - PlayerOldLocation;
	//	Camera->SetWorldLocation(FVector(NewCameraLocation.X, Camera->GetComponentLocation().Y + deff.Y, NewCameraLocation.Z));

	//	// カメラ位置を更新
	//	NewCameraLocation = Camera->GetComponentLocation();
	//}

	// 地上にいるときは上攻撃の当たり判定を無効にする
	if (!GetCharacterMovement()->IsFalling())
	{
		UpperAttackBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	// プレイヤーの現在位置を更新
	PlayerOldLocation = GetActorLocation();
}

// 入力のバインド
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {

		// ジャンプ開始／終了
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &APlayerCharacter::JumpStop);

		// 移動
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Movement);

		// ダッシュ（スペシャルアクション）
		EnhancedInputComponent->BindAction(SpecialAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Action);
		EnhancedInputComponent->BindAction(SpecialAction, ETriggerEvent::Completed, this, &APlayerCharacter::StopAction);
	}
}

bool APlayerCharacter::AssignAttackStrategy(FName AttackID, UAttackStrategy* NewStrategy)
{

	return true;
}

// 上攻撃時のヒット処理
void APlayerCharacter::OnUpperAttack(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!AttackManager)
		return;

	AttackManager->GetAttack("Stomp")->PerformAttack(OtherActor);
}

// 踏みつけ攻撃時のヒット処理
void APlayerCharacter::OnStompAttack(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!AttackManager)
		return;

	AttackManager->GetAttack("Stomp")->PerformAttack(OtherActor);
}

// 移動処理（StateManager 経由）
void APlayerCharacter::Movement(const FInputActionValue& Value)
{
	// 入力値（X = 左右, Y = 前後）
	FVector2D MoveInput = Value.Get<FVector2D>();

	// カメラの回転から前方・右方向ベクトルを取得
	FRotator CamRot = GetControlRotation();
	FVector CamForward = CamRot.Vector(); // 前方ベクトル
	FVector CamRight = FRotationMatrix(CamRot).GetUnitAxis(EAxis::Y); // 右方向ベクトル

	// ========== 実際の移動処理 ==========
	// 入力値に基づく移動方向を計算し、正規化
	FVector MoveDir = (CamRight * MoveInput.X + CamForward * MoveInput.Y).GetSafeNormal();

	// キャラクターを移動させる
	AddMovementInput(MoveDir, StateManager->GetCurrentState()->GetMoveSpeed());

	// 入力がある場合のみ、キャラクターの向きを滑らかに回転させる
	if (!MoveDir.IsNearlyZero())
	{
		// 向くべき方向を計算
		FRotator TargetRot = UKismetMathLibrary::FindLookAtRotation(
			GetActorLocation(),
			GetActorLocation() + MoveDir
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
	if (CanJump())
	{
		ACharacter::Jump();
		UpperAttackBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}
}

// ジャンプ終了処理
void APlayerCharacter::JumpStop(const FInputActionValue& Value)
{
	ACharacter::StopJumping();
}

// ダッシュ開始（移動速度上昇）
void APlayerCharacter::Action(const FInputActionValue& Value)
{
	if (!bIsDashing)
	{
		GetCharacterMovement()->MaxWalkSpeed = 1200.0f;
		bIsDashing = true;
	}
}

// ダッシュ終了（移動速度戻す）
void APlayerCharacter::StopAction()
{
	if (bIsDashing)
	{
		GetCharacterMovement()->MaxWalkSpeed = 600.0f;
		bIsDashing = false;
	}
}

// 状態の変更（タグ指定）
bool APlayerCharacter::ChangeState(FString Tag)
{
	return StateManager->ChangeState(Tag);
}