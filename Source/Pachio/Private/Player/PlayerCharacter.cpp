#include "Player/PlayerCharacter.h"
#include "Player/State/PlayerDefaultState.h"
#include "Player/InGameController.h"
#include "Components/PhysicsCalculator.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/MoveComponent.h"
#include "Components/Color/ColorControllerComponent.h"
#include "Components/Color/ObjectColorComponent.h"
#include "Components/Player/PlayerInputComponent.h"
#include "Components/CameraHandlerComponent.h"
#include "DataContainer/ColorTargetTypes.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "FunctionLibrary.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "InputAction.h"
#include "Interface/Soundable.h"
#include "Interface/StateManager.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h" 
#include "Logic/Movement/PlayerMoveLogic.h"
#include "Manager/LevelManager.h"
#include "Manager/ColorManager.h"
#include "UI/UIManager.h"

namespace
{
	// ============================
	// ==== 定数定義（constexpr） ====
	// ============================

	static constexpr int32 OUTLINE_STENCIL_VALUE = 10;
	static constexpr float MOUSE_COLOR_CHANGE_RATE = 0.01f;
	static constexpr float SCROLL_COLOR_CHANGE_RATE = 0.1f;
	static constexpr float STICK_DEADZONE = 0.02f;
	static constexpr float MOVE_SOUND_INTERVAL = 0.5f;
	static constexpr float MOUSE_DELTA_THRESHOLD = 4.0f;
	static constexpr float GLOW_INTENSITY_ON = 1.0f;
	static constexpr float GLOW_INTENSITY_OFF = 0.0f;
}

// コンストラクタ
APlayerCharacter::APlayerCharacter()
	: JumpForce(12.f)
	, JumpBuff(1.f)
	, MoveSpeed(10.f)
	, MoveSoundCooldown(0.f)
	, DefaultGravityScales(50.f)
	, FixedXLocation(0.f)
	, bHasPrevMouse(false)
	, bHasPrevInputDir(false)
	, PrevInputDir(FVector::ZeroVector)
{
	// 毎フレームTickを実行可能に設定
	PrimaryActorTick.bCanEverTick = true;
	// 各種コンポーネントを生成・初期化
	CameraHandleComponent = CreateDefaultSubobject<UCameraHandlerComponent>(TEXT("CameraComponent"));
	physics = CreateDefaultSubobject<UPhysicsCalculator>(TEXT("Physics"));
	colorController = CreateDefaultSubobject<UColorControllerComponent>(TEXT("ColorController"));
	CurrentManagerComponent = CreateDefaultSubobject<UStateManagerBase>(TEXT("StateManager"));
}

// ゲーム開始時の初期化処理
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	FixedXLocation = GetActorLocation().X;
	// カメラコンポーネントの初期化（ルートコンポーネントを親に設定）
	if (CameraHandleComponent != nullptr)
	{
		CameraHandleComponent->Init(RootComponent);
	}
	// ステート管理・攻撃管理初期化
	ChangeStateManager(EColorCategory::Green);
	// 物理パラメータ設定
	InitPhysicsSettings();
	// 入力設定初期化
	InitInput();
	// 視覚関連設定（アウトラインなど）
	InitVisualSettings();
	// ColorManager に登録
	ALevelManager::GetInstance(GetWorld())->GetColorManager()->RegisterTarget(EColorTargetType::Responders, this);

	bUseControllerRotationYaw = false;

	if (UObjectColorComponent* Occ = GetComponentByClass<UObjectColorComponent>())
	{
		// UObject派生クラスのメンバ関数をバインド
		Occ->OnColorChanged.AddUObject(this, &APlayerCharacter::NotifyStateManagerChange);
	}
}

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CurrentManagerComponent == nullptr)
		return;
	Circle();

	CurrentManagerComponent->Execute_Update(CurrentManagerComponent,DeltaTime);
	UpdateGlowTarget();
	if (GetActorLocation().X != FixedXLocation)
		SetActorLocation(FVector(FixedXLocation, GetActorLocation().Y, GetActorLocation().Z));
}

TScriptInterface<IStateManager> APlayerCharacter::ChangeStateManager(EColorCategory NextStateTag)
{
	AActor* owner = GetOwner();
	if (!owner)
		return nullptr;

	if (StateManagerClass.Num() == 0 || !StateManagerClass.Contains(NextStateTag))
		return nullptr;

	UStateManagerBase* newState = NewObject<UStateManagerBase>(this,StateManagerClass[NextStateTag]);
	if (!newState)
		return nullptr;

	// 新しいステートを生成

	newState->Execute_Init(newState, this, GetWorld());

	CurrentManagerComponent = newState;

	return CurrentManagerComponent;
}
void APlayerCharacter::NotifyStateManagerChange(EColorCategory NewColor)
{
	ChangeStateManager(NewColor);
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
	return CurrentManagerComponent ? CurrentManagerComponent->Execute_GetCurrentState(CurrentManagerComponent) : nullptr;
}


void APlayerCharacter::SetCameraLocation(FVector2D grid, float ZBuffa)
{
	CameraHandleComponent->ApplyCameraSettings(grid, ZBuffa);
}

void APlayerCharacter::ResetBuff()
{
	 JumpBuff = 1;
	 physics->SetGravityScale(true, DefaultGravityScales);
}

void APlayerCharacter::Circle()
{
	float DeltaX, DeltaY;
	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (PC == nullptr) 
		return;

	PC->GetInputMouseDelta(DeltaX, DeltaY);
	FVector2D CurrentDir(DeltaX, DeltaY);

	if (CurrentDir.SizeSquared() > MOUSE_DELTA_THRESHOLD)
	{
		CurrentDir.Normalize();

		if (bHasPrevMouse)
		{
			float CrossZ = PrevMouseDir.X * CurrentDir.Y - PrevMouseDir.Y * CurrentDir.X;

			if (CrossZ > 0)
			{
				ChangeColor(-MOUSE_COLOR_CHANGE_RATE);  // 左回し → -1
			}
			else if (CrossZ < 0)
			{
				ChangeColor(MOUSE_COLOR_CHANGE_RATE); // 右回し → +1
			}
		}

		PrevMouseDir = CurrentDir;
		bHasPrevMouse = true;
	}
}

// 移動入力処理（MoveCompを通して移動方向を取得し移動）
void APlayerCharacter::Movement(const FInputActionValue& Value)
{
	if (CurrentManagerComponent == nullptr)
		return;

	UPlayerStateComponent* CurrentState = CurrentManagerComponent->Execute_GetCurrentState(CurrentManagerComponent);
	if (CurrentState != nullptr)
	{
		CurrentState->Movement(Value);
	}
}

// ジャンプ処理（地面に接地している場合のみ力を加える）
// 移動方向はMovement関数で取得済みのFVector directionをジャンプでも使いたいので
// Movement関数のdirectionをJump関数に渡すか、Jump関数内で再取得する必要あり

void APlayerCharacter::Jump(const FInputActionValue& Value)
{
	if (CurrentManagerComponent == nullptr)
		return;

	UPlayerStateComponent* CurrentState = CurrentManagerComponent->Execute_GetCurrentState(CurrentManagerComponent);
	if (CurrentState != nullptr)
	{
		CurrentState->Jump(JumpForce * JumpBuff);
	}
}

// ダッシュ・スキル開始処理
// APlayerCharacter.cpp 内の Action メソッド
void APlayerCharacter::Action(const FInputActionValue& Value)
{
	if (CurrentManagerComponent == nullptr)
		return;

	UPlayerStateComponent* CurrentState = CurrentManagerComponent->Execute_GetCurrentState(CurrentManagerComponent);
	if (CurrentState != nullptr)
	{
		CurrentState->OnSkill(Value);
	}
}

void APlayerCharacter::SetGravityScale(bool applyGravity)
{
	if (physics == nullptr)
		return;

	physics->SetGravityScale(applyGravity, DefaultGravityScales);
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

	if (ScrollValue > SCROLL_COLOR_CHANGE_RATE)
	{
		ChangeColor(SCROLL_COLOR_CHANGE_RATE);
	}
	else if (ScrollValue < -SCROLL_COLOR_CHANGE_RATE)
	{
		ChangeColor(-SCROLL_COLOR_CHANGE_RATE);
	}
}

void APlayerCharacter::ChangeColor(float value)
{
	if (colorController == nullptr)
		return;

	colorController->AdjustColor(value);
}

void APlayerCharacter::SetColor(float value)
{
	if (colorController == nullptr)
		return;

	colorController->SetColor(value);
}

// カラーモードを右にシフト（次の色モードへ変更）
void APlayerCharacter::ShiftArrayRightColorMode()
{
	if (CameraHandleComponent == nullptr)
		return;

	CameraHandleComponent->ChangeViewMode(ECameraViewType::CharacterView);
}

// カラーモードを左にシフト（前の色モードへ変更）
void APlayerCharacter::ShiftArrayLeftColorMode()
{
	if (CameraHandleComponent == nullptr)
		return;

	CameraHandleComponent->ChangeViewMode(ECameraViewType::GridView);
}

// 状態の変更（ステートタグを指定して遷移）
UPlayerStateComponent* APlayerCharacter::ChangeState(EPlayerStateType Tag)
{
	UPlayerStateComponent* result = CurrentManagerComponent->Execute_ChangeState(CurrentManagerComponent,Tag);
	
	return result;
}



// 物理パラメータの初期化（摩擦・重力設定など）
void APlayerCharacter::InitPhysicsSettings()
{
	physics->RegisterComponent();
	// 重力を加える（値は任意、固定でOUTLINE_STENCIL_VALUEを加算）
	physics->SetGravityScale(true, DefaultGravityScales);

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
		pMesh->SetCustomDepthStencilValue(OUTLINE_STENCIL_VALUE);
	}
}


void APlayerCharacter::OnStickRotate(const FVector2D& StickInput)
{
	//UE_LOG(LogTemp, Log, TEXT("StickInput.x=%f,StickInput.y=%f"), StickInput.X, StickInput.Y);

	const float DeadZone = STICK_DEADZONE;
	if (StickInput.SizeSquared() < DeadZone)
	{
		//UE_LOG(LogTemp, Log, TEXT("StickInput is Neutral"));
		bHasPrevInputDir = false;
		return;
	}

	if (!bHasPrevInputDir)
	{
		float AngleDegrees = FMath::RadiansToDegrees(FMath::Atan2(StickInput.Y, StickInput.X));

		// -180 ~ +180 を 0 ~ 360 に正規化
		if (AngleDegrees < 0)
		{
			AngleDegrees += 360.0f;
		}

		// 基準をX+からY+にしたいなら -90°
		AngleDegrees += 90.0f;

		// 再度0~360に正規化（マイナスになった場合）
		if (AngleDegrees < 0)
		{
			AngleDegrees += 360.0f;
		}
		SetColor(AngleDegrees);

		return;
	}
}

void APlayerCharacter::OnStickMove(const FInputActionValue& Value)
{
	FVector2D StickInput = Value.Get<FVector2D>();
	OnStickRotate(StickInput);
}

void APlayerCharacter::OpenMenu(const FInputActionValue& Value)
{
	ALevelManager::GetInstance(GetWorld())->GetUIManager()->ShowWidget(EWidgetCategory::Menu, "Menu");
}

UCameraComponent* APlayerCharacter::GetCamera()const
{
	if (CameraHandleComponent == nullptr)
		return nullptr;

	return CameraHandleComponent->GetCamera();
}

FVector APlayerCharacter::GetAnimVelocity() const
{
	if (CurrentManagerComponent == nullptr || CurrentManagerComponent->Execute_GetCurrentState(CurrentManagerComponent) == nullptr)
		return FVector();

	return CurrentManagerComponent->Execute_GetCurrentState(CurrentManagerComponent)->GetAnimVelocity();
}

float APlayerCharacter::GetYaw() const
{
	if (CurrentManagerComponent == nullptr || CurrentManagerComponent->Execute_GetCurrentState(CurrentManagerComponent) == nullptr)
		return 0.0f;

	return CurrentManagerComponent->Execute_GetCurrentState(CurrentManagerComponent)->GetYaw();
}

void APlayerCharacter::UpdateGlowTarget()
{
	UBoxComponent* InteractionBox = UFunctionLibrary::FindComponentByName<UBoxComponent>(this, TEXT("InteractionBox"));
	if (InteractionBox == nullptr)
	{
		CurrentGlowTarget = nullptr;
		return;
	}
	// オーバーラップ中のActorを取得
	TArray<AActor*> OverlappingActors;
	InteractionBox->GetOverlappingActors(OverlappingActors);

	AActor* NewGlowTarget = nullptr;
	float MinDistSq = FLT_MAX;
	FVector MyLocation = GetActorLocation();

	for (AActor* Actor : OverlappingActors)
	{
		if (!IsValid(Actor))
			continue;

		// Tagで判定
		if (Actor->ActorHasTag("Holdable"))
		{
			float DistSq = FVector::DistSquared(MyLocation, Actor->GetActorLocation());
			if (DistSq < MinDistSq)
			{
				MinDistSq = DistSq;
				NewGlowTarget = Actor;
			}
		}
	}

	if (NewGlowTarget != CurrentGlowTarget)
	{
		// 前のGlowを解除
		if (CurrentGlowTarget)
		{
			if (USkeletalMeshComponent* mesh = CurrentGlowTarget->FindComponentByClass<USkeletalMeshComponent>())
			{
				mesh->SetScalarParameterValueOnMaterials(TEXT("GlowIntensity"), GLOW_INTENSITY_OFF);
			}
		}

		// 新しいGlowを適用
		if (NewGlowTarget)
		{
			if (USkeletalMeshComponent* mesh = NewGlowTarget->FindComponentByClass<USkeletalMeshComponent>())
			{
				mesh->SetScalarParameterValueOnMaterials(TEXT("GlowIntensity"), GLOW_INTENSITY_ON);
			}
		}

		CurrentGlowTarget = NewGlowTarget;
	}
}
