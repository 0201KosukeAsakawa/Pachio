// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/PlayerCharacter.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Player/State/PlayerDefaultState.h"
//#include "Player/State/DefaultPlayerState.h"

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
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &APlayerCharacter::Jump);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Movement);

		// Looking
		//EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &APachioCharacter::Look);
	}
	else
	{
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
	if (CurrentState != nullptr)
	{
		CurrentState->Jump(Value);
	}
}

void APlayerCharacter::Action(const FInputActionValue& Value)
{

}

