// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/Player/PlayerInputComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Player/PlayerCharacter.h"


void UPlayerInputComponent::Init(TObjectPtr<AController>Controller)
{
	// 入力マッピングコンテキストの追加
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
    
    if (GetOwner()->GetClass()->ImplementsInterface(UControllableInterface::StaticClass()))
    {
        ControllableTarget.SetObject(GetOwner());
        ControllableTarget.SetInterface(
            Cast<IControllableInterface>(GetOwner())
        );
    }
    else
    {
        ControllableTarget = nullptr;
    }
}

// Sets default values for this component's properties
UPlayerInputComponent::UPlayerInputComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

void UPlayerInputComponent::BindInput(UInputComponent* PlayerInputComponent)
{
    UEnhancedInputComponent* EnhancedInputComponent =
        Cast<UEnhancedInputComponent>(PlayerInputComponent);
    if (!EnhancedInputComponent) return;

    EnhancedInputComponent->BindAction(
        MoveAction,
        ETriggerEvent::Triggered,
        this,
        &UPlayerInputComponent::HandleMove
    );

    EnhancedInputComponent->BindAction(
        JumpAction,
        ETriggerEvent::Started,
        this,
        &UPlayerInputComponent::HandleJump
    );

    EnhancedInputComponent->BindAction(
        SpecialAction,
        ETriggerEvent::Triggered,
        this,
        &UPlayerInputComponent::HandleAction
    );

    EnhancedInputComponent->BindAction(
        IncreaseColorAction,
        ETriggerEvent::Triggered,
        this,
        &UPlayerInputComponent::HandleIncreaseColor
    );

    EnhancedInputComponent->BindAction(
        ShiftArrayRightColorAction,
        ETriggerEvent::Triggered,
        this,
        &UPlayerInputComponent::HandleShiftColorRight
    );

    EnhancedInputComponent->BindAction(
        ShiftArrayLeftColorAction,
        ETriggerEvent::Triggered,
        this,
        &UPlayerInputComponent::HandleShiftColorLeft
    );

    EnhancedInputComponent->BindAction(
        StickAction,
        ETriggerEvent::Triggered,
        this,
        &UPlayerInputComponent::HandleStick
    );

    EnhancedInputComponent->BindAction(
        Option,
        ETriggerEvent::Triggered,
        this,
        &UPlayerInputComponent::HandleOpenMenu
    );
}

void UPlayerInputComponent::HandleMove(const FInputActionValue& Value)
{
    if (ControllableTarget)
    {
        ControllableTarget->Movement(Value);
    }
}

void UPlayerInputComponent::HandleJump(const FInputActionValue& Value)
{
    if (ControllableTarget)
    {
        ControllableTarget->Jump(Value);
    }
}

void UPlayerInputComponent::HandleAction(const FInputActionValue& Value)
{
    if (ControllableTarget)
    {
        ControllableTarget->Action(Value);
    }
}

void UPlayerInputComponent::HandleIncreaseColor(const FInputActionValue& Value)
{
    if (ControllableTarget)
    {
        ControllableTarget->ChangeColor(Value.Get<float>());
    }
}

void UPlayerInputComponent::HandleShiftColorRight(const FInputActionValue&)
{
    if (ControllableTarget)
    {
        ControllableTarget->ChangeCameraViewModeToCharacter();
    }
}

void UPlayerInputComponent::HandleShiftColorLeft(const FInputActionValue&)
{
    if (ControllableTarget)
    {
        ControllableTarget->ChangeCameraViewModeToGrid();
    }
}

void UPlayerInputComponent::HandleStick(const FInputActionValue& Value)
{
    if (ControllableTarget)
    {
        ControllableTarget->OnStickMove(Value);
    }
}

void UPlayerInputComponent::HandleOpenMenu(const FInputActionValue& Value)
{
    if (ControllableTarget)
    {
        ControllableTarget->OpenMenu(Value);
    }
}
