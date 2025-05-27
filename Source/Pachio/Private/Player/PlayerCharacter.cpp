// Copyright notice を Description ページで記載

// インクルード

#include "Player/PlayerCharacter.h"
#include "Components/PhysicsCalculator.h"
#include "Components/StaticMeshComponent.h"
#include "Components/AttackComponent.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/AttackManagerComponent.h"
#include "Components/MoveComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "FunctionLibrary.h"
#include "InputAction.h"
#include "Kismet/KismetMathLibrary.h"
#include "Player/State/PlayerDefaultState.h"
#include "Player/State/StateManager.h"
#include "Logic/Movement/PlayerMoveLogic.h"


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

	MoveComp =  NewObject<UMoveComponent>(this);
	UPlayerMoveLogic* PlayerLogic = NewObject<UPlayerMoveLogic>(this);
	MoveComp->Init(this,PlayerLogic);



	bIsDashing = false; // 初期状態ではダッシュしていない

		// 攻撃コンポーネントの生成
	AttackManager = NewObject<UAttackManagerComponent>(this);
	StateManager = NewObject<UStateManager>(this, StateManagerClass);

	physics = NewObject<UPhysicsCalculator>(this);
	physics->RegisterComponent();            // Tick対象になる

	if (!AttackManager || !StateManagerClass)
		return;

	AttackManager->Init(GetWorld());
	AttackManager->ResetMap();
	AttackManager->RegisterAttackComponent("Stomp");
	AttackManager->RegisterAttackComponent("Upper");
	StateManager->RegisterComponent(); // Register as component
	StateManager->Init(this, GetWorld());


	// 攻撃判定用のボックスコンポーネントを探す
	UpperAttackBox = UFunctionLibrary::FindComponentByName<UBoxComponent>(this, "Upper");
	StompAttackBox = UFunctionLibrary::FindComponentByName<UBoxComponent>(this, "Stomp");

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

	//プレイヤーの座標を保存
	PlayerOldLocation = GetActorLocation();

	// カメラのY座標最大値を初期化（初期カメラ位置）
	if (Camera)
	{
		//カメラの初期位置を少し右にずれるようにして設定
		MaxCameraY = Camera->GetComponentLocation().Y + 1000;
		CameraXZ = Camera->GetComponentLocation();

		// カメラのY座標を固定（右スクロール固定）
		FVector CameraLocation = Camera->GetComponentLocation();

		CameraLocation = FVector(CameraXZ.X, MaxCameraY, CameraXZ.Z);
		Camera->SetWorldLocation(CameraLocation);
	}

	// 入力マッピングコンテキストの追加
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
	GetCharacterMovement()->BrakingFrictionFactor = 2.0f; // 止まる速さを上げる
	GetCharacterMovement()->GroundFriction = 8.0f; // 地面との摩擦を強化
	// 重力スケールを強化（より素早い落下）
	GetCharacterMovement()->GravityScale = 0.0f;
}

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//攻撃の判定が消えていたら即時リターン
	if (!UpperAttackBox || !StateManager)
		return;

	//ステートマネージャーの経過処理呼び出し
	StateManager->Update(DeltaTime);

	//プレイヤーのY座標取得
	float PlayerY = GetActorLocation().Y;

	if (SpringArm && Camera)
	{
		// カメラの最大Y座標を更新（右スクロールのみ）
		if (PlayerY > MaxCameraY)
		{
			MaxCameraY = PlayerY;
		}

		// カメラのY座標を固定（右スクロール固定）
		FVector CameraLocation = Camera->GetComponentLocation();
		
		CameraLocation = FVector(CameraXZ.X, MaxCameraY, CameraXZ.Z);
		Camera->SetWorldLocation(CameraLocation);

		// ===== プレイヤーの制限 =====
		FVector Location = GetActorLocation();

		// カメラの左端を計算（横スクロール用）
		const float HalfScreenWidth = 1200.0f; // 実際のゲームに応じて調整
		float CameraLeftEdgeY = MaxCameraY - HalfScreenWidth;

		// 左に行きすぎたら止める
		if (Location.Y < CameraLeftEdgeY)
		{
			Location.Y = CameraLeftEdgeY;
			SetActorLocation(Location);
		}
	}

	// 空中ならば攻撃判定を有効にする
	if (GetCharacterMovement()->IsFalling())
	{
		UpperAttackBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		StompAttackBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}
	// 空中でなければ攻撃判定を無効にする
	if (!GetCharacterMovement()->IsFalling())
	{
		UpperAttackBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		StompAttackBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	//プレイヤーが無敵時間ならば処理する
	if (bIsInvincible)
	{
		UpdateInvincible(DeltaTime);
	}
	physics->AddGravity(10.0f);
	PlayerOldLocation = GetActorLocation();
}

// 入力のバインド
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {

		// ジャンプ開始／終了
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &APlayerCharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &APlayerCharacter::JumpStop);

		// 移動
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Movement);

		//しゃがみ
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Crouch);
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Completed, this, &APlayerCharacter::StandUp);

		// ダッシュ（スペシャルアクション）
		EnhancedInputComponent->BindAction(SpecialAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Action);
		EnhancedInputComponent->BindAction(SpecialAction, ETriggerEvent::Completed, this, &APlayerCharacter::StopAction);
	}
}

bool APlayerCharacter::AssignAttackStrategy(FName AttackID, UAttackStrategy* NewStrategy)
{
	return true;
}

UPlayerStateComponent* APlayerCharacter::GetPlayerState() const
{
	return StateManager->GetCurrentState();
}

// 上攻撃時のヒット処理
void APlayerCharacter::OnUpperAttack(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	/*UE_LOG(LogTemp, Log, TEXT("UPPER!"));
	if (!AttackManager || !OtherActor || OtherActor == this)
		return;

	if (!AttackManager->GetAttack("Upper")->PerformAttack(OtherActor))
		return;*/


	physics->ResetForce();
}

// 踏みつけ攻撃時のヒット処理
void APlayerCharacter::OnStompAttack(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Log, TEXT("Stomp!"));
	if (!AttackManager || !OtherActor || OtherActor == this)
		return;

	if (AttackManager->GetAttack("Stomp")->PerformAttack(OtherActor))
	{
		physics->AddForce(GetActorUpVector(), 3);
	}
}

bool APlayerCharacter::TakeDamage(FAttackData Data, float damage , const AActor*)
{
	if (bIsInvincible)
		return false; // 無敵状態の場合、ダメージを無視

	if (!StateManager)
		return false;

	// 無敵時間開始
	bIsInvincible = true;
	InvincibleTime = MaxInvincibleTime;

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

void APlayerCharacter::Crouch(const FInputActionValue& Value)
{
	if (!UpperAttackBox || !StompAttackBox)
		return;

	if (Cast<UPlayerDefaultState>(StateManager->GetCurrentState()))
		return;

	PowerDownCollisionPosition();
	
	//コリジョンのサイズ変更
	GetCharacterMovement()->Crouch();
	GetCapsuleComponent()->SetCapsuleHalfHeight(55.0);
}

void APlayerCharacter::StandUp()
{
	if (Cast<UPlayerDefaultState>(StateManager->GetCurrentState()))
		return;

	PowerUpCollisionPosition();
	//コリジョンのサイズ変更
	GetCharacterMovement()->Crouch();
	GetCapsuleComponent()->SetCapsuleHalfHeight(110.0);
}

// ジャンプ処理（ジャンプ中に上攻撃の判定を有効化）
void APlayerCharacter::Jump(const FInputActionValue& Value)
{
	//ジャンプが可能な状態なら
	if (physics->OnGround(GetActorLocation()))
		physics->AddForce(GetActorUpVector(), 10, false);
}

// ジャンプ終了処理
void APlayerCharacter::JumpStop(const FInputActionValue& Value)
{
	ACharacter::StopJumping();
}

// ダッシュ開始（移動速度上昇）
void APlayerCharacter::Action(const FInputActionValue& Value)
{
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
	}
}

//パワーアップ時にコリジョンの移動処理
void APlayerCharacter::PowerUpCollisionPosition()
{
	//即時リターン
	if (!UpperAttackBox || !StompAttackBox)
		return;

	//上と下の攻撃判定を拡大調整
	UpperAttackBox->SetRelativeLocation(FVector(0, 0, 110));
	StompAttackBox->SetRelativeLocation(FVector(0, 0, -110));
}

//パワーダウン時にコリジョンの移動処理
void APlayerCharacter::PowerDownCollisionPosition()
{
	//即時リターン
	if (!UpperAttackBox || !StompAttackBox)
		return;

	//上と下の攻撃判定を縮小調整
	UpperAttackBox->SetRelativeLocation(FVector(0, 0, 55));
	StompAttackBox->SetRelativeLocation(FVector(0, 0, -55));
}

void APlayerCharacter::ToggleVisibility()
{
	UStaticMeshComponent* pMesh = UFunctionLibrary::FindComponentByName<UStaticMeshComponent>(this, "StaticMesh");
	if (pMesh == nullptr)
		return;

	if (bIsVisible)
	{
		pMesh->SetVisibility(false);
	}
	else
	{
		pMesh->SetVisibility(true);
	}

	// 状態を反転
	bIsVisible = !bIsVisible;
}

void APlayerCharacter::UpdateInvincible(float DeltaTime)
{
	UStaticMeshComponent* pMesh = UFunctionLibrary::FindComponentByName<UStaticMeshComponent>(this, "StaticMesh");
	if (pMesh == nullptr)
		return;

	// 無敵時間を減少させる
	InvincibleTime -= DeltaTime;

	if (InvincibleTime <= 0.0f)
	{
		bIsInvincible = false; // 無敵時間終了
		GetWorld()->GetTimerManager().ClearTimer(BlinkTimerHandle);  // タイマーの停止
		pMesh->SetVisibility(true);  // 最後にメッシュを表示状態に戻す
	}
	else
	{
		// 点滅の処理
		if (!GetWorld()->GetTimerManager().IsTimerActive(BlinkTimerHandle))
		{
			// タイマーを設定して、定期的に点滅させる
			GetWorld()->GetTimerManager().SetTimer(BlinkTimerHandle, this, &APlayerCharacter::ToggleVisibility, 0.1f, true);
		}
	}
}

// 状態の変更（タグ指定）
bool APlayerCharacter::ChangeState(FString Tag)
{
	return StateManager->ChangeState(Tag);
}