// Fill out your copyright notice in the Description page of Project Settings.


#include "Attack/UpperAttackStrategy.h"
#include "Interface/IDamageable.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

void UUpperAttackStrategy::ExecuteEffect(AActor* Attacker, AActor* Target, FAttackData attackData, float FinalDamage)
{
	IDamageable* id = Cast<IDamageable>(Target);
	if (!id)
		return;

	// ダメージが通ったら（ブロックが破壊された等）、慣性を止める
	if (id->TakeDamage(attackData, FinalDamage))
	{
		if (ACharacter* Character = Cast<ACharacter>(Attacker))
		{
			Character->GetCharacterMovement()->StopMovementImmediately();

			// もしくは特定の方向に少し跳ね返るなども可能
			 FVector BounceBack = FVector(0, 0, 300); 
			 Character->LaunchCharacter(BounceBack, true, true);
		}
	}
}