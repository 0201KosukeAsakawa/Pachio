// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/PlayerInputComponent.h"
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
}

// Sets default values for this component's properties
UPlayerInputComponent::UPlayerInputComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

void UPlayerInputComponent::BindInput(UInputComponent* PlayerInputComponent )
{
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(GetOwner());
		if (!PlayerCharacter) return;

		// ジャンプ
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, PlayerCharacter, &APlayerCharacter::Jump);

		// 移動
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, PlayerCharacter, &APlayerCharacter::Movement);

		// アクション
		EnhancedInputComponent->BindAction(SpecialAction, ETriggerEvent::Triggered, PlayerCharacter, &APlayerCharacter::Action);
		EnhancedInputComponent->BindAction(SpecialAction, ETriggerEvent::Completed, PlayerCharacter, &APlayerCharacter::StopAction);

		// カラーモード操作
		EnhancedInputComponent->BindAction(IncreaseColorAction, ETriggerEvent::Triggered, PlayerCharacter, &APlayerCharacter::OnMouseScroll);
		//EnhancedInputComponent->BindAction(DecreaseColorAction, ETriggerEvent::Triggered, PlayerCharacter, &APlayerCharacter::DecreaseColor);

		EnhancedInputComponent->BindAction(ShiftArrayRightColorAction, ETriggerEvent::Triggered, PlayerCharacter, &APlayerCharacter::ShiftArrayRightColorMode);
		EnhancedInputComponent->BindAction(ShiftArrayLeftColorAction, ETriggerEvent::Triggered, PlayerCharacter, &APlayerCharacter::ShiftArrayLeftColorMode);
	}

}
