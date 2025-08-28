#include "Player/PlayerCharacter.h"
#include "Player/State/PlayerDefaultState.h"
#include "Player/State/StateManager.h"
#include "Player/InGameController.h"
#include "Components/PhysicsCalculator.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/MoveComponent.h"
#include "Components/Color/ColorControllerComponent.h"
#include "Components/Player/PlayerInputComponent.h"
#include "Components/CameraHandlerComponent.h"
#include "Components/InvincibilityComponent.h"
#include "DataContainer/EffectMatchResult.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "FunctionLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "InputAction.h"
#include "Interface/Soundable.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h" 
#include "Logic/Movement/PlayerMoveLogic.h"
#include "Manager/LevelManager.h"
#include "Manager/ColorManager.h"
#include "Objects/ControllableObjectBase.h"
#include "UI/UIManager.h"




// コンストラクタ
APlayerCharacter::APlayerCharacter()
{
	// 毎フレームTickを実行可能に設定
	PrimaryActorTick.bCanEverTick = true;
	// 各種コンポーネントを生成・初期化
	/*InteractionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("IBox"));*/
	CameraComponent = CreateDefaultSubobject<UCameraHandlerComponent>(TEXT("CameraComponent"));
	physics = CreateDefaultSubobject<UPhysicsCalculator>(TEXT("Physics"));
	colorController = CreateDefaultSubobject<UColorControllerComponent>(TEXT("ColorController"));
	InvincibilityComponent = CreateDefaultSubobject<UInvincibilityComponent>(TEXT("InvincibilityComponent"));


}

// ゲーム開始時の初期化処理
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	// カメラコンポーネントの初期化（ルートコンポーネントを親に設定）
	CameraComponent->Init(RootComponent);
	// ステート管理・攻撃管理初期化
	InitState();
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
	UBoxComponent* box = UFunctionLibrary::FindComponentByName<UBoxComponent>(this, TEXT("Interaction"));
	if(box)
	{
		InteractionBox = box;
	}
}

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!StateManager)
		return;

	HandleMoveSound(DeltaTime);
	UpdateOverlapUI();

	Circle();

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
		PlayerInputData->BindInput<APlayerCharacter>(PlayerInputComponent);
	}
}

// 現在のプレイヤーステートを取得
UPlayerStateComponent* APlayerCharacter::GetPlayerState() const
{
	return StateManager->GetCurrentState();
}

void APlayerCharacter::SetCameraLocation(FVector2D grid, float ZBuffa)
{
	CameraComponent->ApplyCameraSettings(grid, ZBuffa);
}

void APlayerCharacter::ResetBuff()
{
	 JumpBuff = 1;
	 GetCharacterMovement()->GroundFriction = 8.0f;
	 GetCharacterMovement()->BrakingDecelerationWalking = 2048.f;
	 physics->SetGravityScale(true, DefaultGravityScalse);
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
	StateManager->GetCurrentState()->Movement(Value);
}

// ジャンプ処理（地面に接地している場合のみ力を加える）
// 移動方向はMovement関数で取得済みのFVector directionをジャンプでも使いたいので
// Movement関数のdirectionをJump関数に渡すか、Jump関数内で再取得する必要あり

void APlayerCharacter::Jump(const FInputActionValue& Value)
{
	StateManager->GetCurrentState()->Jump(physics, JumpForce * JumpBuff);
}

// ダッシュ・スキル開始処理
// APlayerCharacter.cpp 内の Action メソッド
void APlayerCharacter::Action(const FInputActionValue& Value)
{
	StateManager->GetCurrentState()->OnSkill(Value);
}


void APlayerCharacter::SetGravityScale(bool applyGravity)
{
	if (physics == nullptr)
		return;

	physics->SetGravityScale(applyGravity, DefaultGravityScalse);
}

void APlayerCharacter::SetGravityScale(bool applyGravity, float scale)
{
	if (physics == nullptr)
		return;

	physics->SetGravityScale(applyGravity, scale);
}

void APlayerCharacter::OnMouseScroll(const FInputActionValue& Value)
{
	float ScrollValue = Value.Get<float>();

	if (ScrollValue > 0.1f)
	{
		ChangeColor(0.1);
	}
	else if (ScrollValue < -0.1f)
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
UPlayerStateComponent* APlayerCharacter::ChangeState(FString Tag)
{
	UPlayerStateComponent* result = StateManager->ChangeState(Tag);
	
	return result;
}

// ステート管理・攻撃管理の初期化
void APlayerCharacter::InitState()
{
	// StateManager を指定のクラスで生成
	StateManager = NewObject<UStateManager>(this, StateManagerClass);

	if ( !StateManagerClass)
		return;

	// ステートマネージャーのコンポーネント登録・初期化
	StateManager->RegisterComponent();
	StateManager->Init(this, GetWorld());
}

// 物理パラメータの初期化（摩擦・重力設定など）
void APlayerCharacter::InitPhysicsSettings()
{
	physics->RegisterComponent();
	// 重力を加える（値は任意、固定で10.0fを加算）
	physics->SetGravityScale(true, DefaultGravityScalse);

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

void APlayerCharacter::ApplyEffectFromColor(const FLinearColor& Color)
{
	// 色から最も近いバフ効果と強度を取得
	FEffectMatchResult Match = ALevelManager::GetInstance(GetWorld())
		->GetColorManager()
		->GetClosestEffectByHue(Color);
	ResetBuff();
	switch (Match.ClosestEffect)
	{
	case EBuffEffect::Red:
	{
		break;
	}

	case EBuffEffect::Green:
	{
		GetCharacterMovement()->MaxWalkSpeed = 1000.0f + 400.0f * Match.StrengthRatio;
		break;
	}

	case EBuffEffect::Blue:
	{
		GetCharacterMovement()->GroundFriction = 0.1f;
		GetCharacterMovement()->BrakingDecelerationWalking =100; // 通常:2048 → 小さいほど止まりにくい

		break;
	}

	default:
	{
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
			ChangeColor(-0.01);
			PrevInputDir = InputDir;
		}
		else if (CrossZ < -epsilon)
		{
			UE_LOG(LogTemp, Log, TEXT("回転方向：右回り（時計回り）"));
			ChangeColor(0.01);
			PrevInputDir = InputDir;
		}

	}
void APlayerCharacter::OnStickMove(const FInputActionValue& Value)
{
	FVector2D StickInput = Value.Get<FVector2D>();
	OnStickRotate(StickInput);
}

void APlayerCharacter::CallOnClosestOverlappingActor()
{
	if (!InteractionBox)
		return;

	TArray<AActor*> OverlappingActors;
	InteractionBox->GetOverlappingActors(OverlappingActors);

	AControllableObjectBase* ClosestActor = nullptr;
	float MinDistanceSq = FLT_MAX;
	FVector MyLocation = GetActorLocation();

	for (AActor* Actor : OverlappingActors)
	{
		if (!IsValid(Actor))
			continue;

		if (AControllableObjectBase* CastedActor = Cast<AControllableObjectBase>(Actor))
		{
			float DistSq = FVector::DistSquared(MyLocation, CastedActor->GetActorLocation());
			if (DistSq < MinDistanceSq)
			{
				MinDistanceSq = DistSq;
				ClosestActor = CastedActor;
			}
		}
	}

	if (ClosestActor)
	{
		ClosestActor->SwitchControll(this);
		// 呼んだら終わり
		return;
	}
}

void APlayerCharacter::UpdateOverlapUI()
{
	if (!InteractionBox)
		return;

	TArray<AActor*> OverlappingActors;
	InteractionBox->GetOverlappingActors(OverlappingActors);

	AActor* OverlappedActor = nullptr;
	for (AActor* Actor : OverlappingActors)
	{
		if (IsValid(Actor) && Actor->IsA(AControllableObjectBase::StaticClass()))
		{
			OverlappedActor = Actor;
			break;
		}
	}

	if (OverlappedActor)
	{
		ALevelManager::GetInstance(GetWorld())->GetUIManager()
		->ShowMarker(TEXT("ChageMovemet"), this);
	}
	else
	{
		ALevelManager::GetInstance(GetWorld())->GetUIManager()
			->HideMarker(TEXT("ChageMovemet"));
	}
}

void APlayerCharacter::HandleMoveSound(float DeltaTime)
{
	FVector Velocity = GetVelocity();
	bool bIsMoving = Velocity.SizeSquared() > KINDA_SMALL_NUMBER;

	ISoundable* sound = ALevelManager::GetInstance(GetWorld())->GetSoundManager().GetInterface();
	if (!sound)
		return;

	if (bIsMoving)
	{
		MoveSoundCooldown -= DeltaTime;
		if (MoveSoundCooldown <= 0.f)
		{
			sound->PlaySound("SE", "MoveStep");  // ループしないSEをここで再生
			MoveSoundCooldown = MoveSoundInterval;
		}
	}
	else
	{
		MoveSoundCooldown = 0.f; // 移動してない時はリセット
	}
}