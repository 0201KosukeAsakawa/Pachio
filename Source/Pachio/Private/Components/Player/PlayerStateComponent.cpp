// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/Player/PlayerStateComponent.h"
#include "InputActionValue.h"

// Sets default values for this component's properties
UPlayerStateComponent::UPlayerStateComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

bool UPlayerStateComponent::OnEnter(ACharacter*, UWorld*)
{
	return false;
}

bool UPlayerStateComponent::OnUpdate(float)
{
	return false;
}

bool UPlayerStateComponent::OnExit(ACharacter*)
{
	return false;
}

bool UPlayerStateComponent::OnSkill(const FInputActionValue&)
{
	return false;
}

void UPlayerStateComponent::Movement(const FInputActionValue& Value)
{
}

bool UPlayerStateComponent::Jump(float jumpForce)
{
	return false;
}

FVector UPlayerStateComponent::GetAnimVelocity() const
{
	float DeltaSeconds = GetWorld()->GetDeltaSeconds();
	FVector Velocity = DeltaSeconds > 0 ? MoveDelta / DeltaSeconds : FVector::ZeroVector;

	UE_LOG(LogTemp, Log, TEXT("AnimVelocity = X:%f, Y:%f, Z:%f | MoveDelta = X:%f, Y:%f, Z:%f | DeltaSeconds = %f"),
		Velocity.X, Velocity.Y, Velocity.Z,
		MoveDelta.X, MoveDelta.Y, MoveDelta.Z,
		DeltaSeconds);

	return Velocity;
}


int32 UPlayerStateComponent::GetYaw() const
{
	return MoveDirection;
}
