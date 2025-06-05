// Fill out your copyright notice in the Description page of Project Settings.


#include "Attack/UpperAttackStrategy.h"
#include "Interface/IDamageable.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

bool UUpperAttackStrategy::ExecuteEffect(AActor* Attacker, AActor* Target, FAttackData attackData, float FinalDamage)
{
	IDamageable* id = Cast<IDamageable>(Target);
	if (!id || !id->TakeDamage(attackData, FinalDamage, Attacker))
		return false;

	ACharacter* Character = Cast<ACharacter>(Attacker);

	// ダメージが通ったら（ブロックが破壊された等）、慣性を止める
	if (!Character)
		return false;

	return true;
}