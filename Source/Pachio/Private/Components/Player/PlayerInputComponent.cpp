#include "Components/Player/PlayerInputComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Player/PlayerCharacter.h"

void UPlayerInputComponent::Init(TObjectPtr<AController>Controller)
{
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
		ControllableTarget.SetInterface(Cast<IControllableInterface>(GetOwner()));
	}
	else
	{
		ControllableTarget = nullptr;
	}
}

UPlayerInputComponent::UPlayerInputComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UPlayerInputComponent::BindInput(UInputComponent* PlayerInputComponent)
{
	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (!EnhancedInputComponent) return;

	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &UPlayerInputComponent::HandleMove);
	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &UPlayerInputComponent::HandleJump);
	EnhancedInputComponent->BindAction(SpecialAction, ETriggerEvent::Triggered, this, &UPlayerInputComponent::HandleAction);
	EnhancedInputComponent->BindAction(IncreaseColorAction, ETriggerEvent::Triggered, this, &UPlayerInputComponent::HandleIncreaseColor);
	EnhancedInputComponent->BindAction(ShiftArrayRightColorAction, ETriggerEvent::Triggered, this, &UPlayerInputComponent::HandleShiftColorRight);
	EnhancedInputComponent->BindAction(ShiftArrayLeftColorAction, ETriggerEvent::Triggered, this, &UPlayerInputComponent::HandleShiftColorLeft);
	EnhancedInputComponent->BindAction(StickAction, ETriggerEvent::Triggered, this, &UPlayerInputComponent::HandleStick);
	EnhancedInputComponent->BindAction(Option, ETriggerEvent::Triggered, this, &UPlayerInputComponent::HandleOpenMenu);

	// ★新規追加:Tank切り替えバインディング
	EnhancedInputComponent->BindAction(SwitchTankNextAction, ETriggerEvent::Triggered, this, &UPlayerInputComponent::HandleSwitchTankNext);
	EnhancedInputComponent->BindAction(SwitchTankPreviousAction, ETriggerEvent::Triggered, this, &UPlayerInputComponent::HandleSwitchTankPrevious);
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

void UPlayerInputComponent::HandleSwitchTankNext(const FInputActionValue& Value)
{
	if (ControllableTarget)
	{
		ControllableTarget->SwitchColorTankNext(Value);
	}
}

void UPlayerInputComponent::HandleSwitchTankPrevious(const FInputActionValue& Value)
{
	if (ControllableTarget)
	{
		ControllableTarget->SwitchColorTankPrevious(Value);
	}
}