// Fill out your copyright notice in the Description page of Project Settings.

//インクルード
#include "Player/PlayerCharacter.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/BoxComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Player/State/PlayerDefaultState.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "FunctionLibrary.h"
#include "Player/State/StateManager.h"

// Sets default values
APlayerCharacter::APlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// SpringArmを作成してルートにアタッチ
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent); // ← これでキャラクターにアタッチされる

	// Cameraを作成してSpringArmにアタッチ
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm); // ← これでスプリングアームの先にカメラが付きます

	SpringArm->TargetArmLength = 500.0f; // カメラ距離

	// キャラの少し右側から見るようにオフセット調整
	SpringArm->SocketOffset = FVector(0.0f, 100.0f, 50.0f); // Yを+100にするとキャラが画面の左寄りに見える
	SpringArm->bUsePawnControlRotation = false;

	//UBoxComponent* CollisionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	// 攻撃判定用ボックスの作成
	UpperAttackBox = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	UpperAttackBox->SetupAttachment(RootComponent);
	// 判定を無効化（当たり判定が発生しないようにする）
	UpperAttackBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	bIsDashing = false;

	manager = NewObject<UStateManager>();
	manager->Init(this,GetWorld());

	// SpringArmの設定: Yaw（左右）方向は追従、Pitch（上下）方向は追従しない
	if (SpringArm)
	{
		SpringArm->bInheritYaw = true;  // 左右の回転を追従
		SpringArm->bInheritPitch = false;  // 上下の回転を追従しない
		SpringArm->bInheritRoll = false;  // ロール（横の回転）を追従しない
	}

	NewCameraLocation = Camera->GetComponentLocation();
	PlayerOldLocation = GetActorLocation();

	// Input Action の設定を行います
	if (JumpAction)
	{
		// InputAction はアセットとして設定されている前提
		JumpAction = LoadObject<UInputAction>(nullptr, TEXT("InputAction'/Game/Path/To/IA_Jump.IA_Jump'"));
	}

	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
	GetCharacterMovement()->GravityScale = 3.0f;
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// カメラの位置を x 軸のみに追従させる
	if (SpringArm && Camera)
	{
		Camera->SetWorldLocation(NewCameraLocation);  // 更新した位置をカメラに反映
		//Camera->SetWorldLocation(FVector(Camera->GetComponentLocation().X, NewCameraLocation.Y, NewCameraLocation.Z));
		
		FVector deff = GetActorLocation() - PlayerOldLocation;
		Camera->SetWorldLocation(FVector(NewCameraLocation.X, Camera->GetComponentLocation().Y + deff.Y,NewCameraLocation.Z));
		NewCameraLocation = Camera->GetComponentLocation();
	}

	if (!GetCharacterMovement()->IsFalling())
	{
		UpperAttackBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	
	PlayerOldLocation = GetActorLocation();
}

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {

		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Jump);
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &APlayerCharacter::JumpStop);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Movement);

		// Looking
		//EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &APachioCharacter::Look);

		//Action
		EnhancedInputComponent->BindAction(SpecialAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Action);
		EnhancedInputComponent->BindAction(SpecialAction, ETriggerEvent::Completed, this, &APlayerCharacter::StopAction);

		//else
		{
		}
	}

}

void APlayerCharacter::GenerateState()
{
	//StateMap.Add("Defaul",TScriptInterface<IStateBase>(NewObject<DefaultPlayerState>()))
}

void APlayerCharacter::Movement(const FInputActionValue& Value)
{
	if (!manager)
		return;

	manager->Movement(Value);
}

void APlayerCharacter::Jump(const FInputActionValue& Value)
{
	/*if (CurrentState != nullptr)
	{
		CurrentState->Jump(Value);
	}*/
	if (CanJump())
	{
		ACharacter::Jump();
		UpperAttackBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}

}

void APlayerCharacter::JumpStop(const FInputActionValue& Value)
{
	ACharacter::StopJumping();
}

void APlayerCharacter::Action(const FInputActionValue& Value)
{
	if (!bIsDashing)
	{
		GetCharacterMovement()->MaxWalkSpeed = 1200.0f;
		bIsDashing = true;
	}
}

void APlayerCharacter::StopAction()
{
	if (bIsDashing)
	{
		GetCharacterMovement()->MaxWalkSpeed = 600.0f;
		bIsDashing = false;
	}
}

void APlayerCharacter::ChangeState(FString Tag)
{
	manager->ChangeState(Tag);
}