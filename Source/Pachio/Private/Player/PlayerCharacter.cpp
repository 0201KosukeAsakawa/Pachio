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
#include "Components/AttackComponent.h"

APlayerCharacter::APlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// SpringArmを作成してルートにアタッチ
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);

	// Cameraを作成してSpringArmにアタッチ
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);

	SpringArm->TargetArmLength = 500.0f;

	SpringArm->SocketOffset = FVector(0.0f, 100.0f, 50.0f);
	SpringArm->bUsePawnControlRotation = false;

	
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	bIsDashing = false;

	//中のステートを初期化していない
	Upper = NewObject<UAttackComponent>(this);
	Stomp = NewObject<UAttackComponent>(this);

	manager = NewObject<UStateManager>();
	manager->Init(this,GetWorld());

	//攻撃判定用ボックスの作成
	UpperAttackBox = UFunctionLibrary::FindComponentByName<UBoxComponent>(this, "A");

	StompAttackBox = UFunctionLibrary::FindComponentByName<UBoxComponent>(this, "B");

	if (!UpperAttackBox || !StompAttackBox)
		return;
	UpperAttackBox->OnComponentBeginOverlap.AddDynamic(this, &APlayerCharacter::OnUpperAttack);
	StompAttackBox->OnComponentBeginOverlap.AddDynamic(this, &APlayerCharacter::OnStompAttack);

	UpperAttackBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	StompAttackBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);

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

void APlayerCharacter::OnUpperAttack(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!Upper)
		return;

	Upper->PerformAttack(OtherActor);
}

void APlayerCharacter::OnStompAttack(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!Stomp)
		return;

	Stomp->PerformAttack(OtherActor);
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