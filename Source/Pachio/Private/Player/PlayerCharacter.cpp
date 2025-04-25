// Fill out your copyright notice in the Description page of Project Settings.

//インクルード
#include "Player/PlayerCharacter.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Player/State/PlayerDefaultState.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "FunctionLibrary.h"

// Sets default values
APlayerCharacter::APlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	//DefaultPlayerState* DP = NewObject<DefaultPlayerState>();
	CurrentState = NewObject<UPlayerDefaultState>();
	if (CurrentState != nullptr)
	{
		CurrentState->OnEnter(this,GetWorld());
	}

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
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (CurrentState != nullptr)
	{
		CurrentState->OnUpdate(this);
	}
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
	if (CurrentState != nullptr)
	{
		CurrentState->Movement(Value);
	}
}

void APlayerCharacter::Jump(const FInputActionValue& Value)
{
	/*/if (CurrentState != nullptr)
	{
		CurrentState->Jump(Value);
	}/*/
	//GetCharacterMovement()->AddForce(FVector(0,0,5000000));

	// CanJump() が true の場合のみジャンプ処理を実行
	if (CanJump())
	{
		ACharacter::Jump();
	}
}

void APlayerCharacter::JumpStop(const FInputActionValue& Value)
{
	ACharacter::StopJumping();
}

void APlayerCharacter::Action(const FInputActionValue& Value)
{

}

