// Fill out your copyright notice in the Description page of Project Settings.


#include "Attack/UpperAttackStrategy.h"
#include "Interface/IDamageable.h"

void UUpperAttackStrategy::ExecuteEffect(AActor* Attacker, AActor* Target, FAttackData attackData, float FinalDamage)
{
	IDamageable* id = Cast<IDamageable>(Target);
	if (!id)
		return;

	if (!id->TakeDamage(attackData, FinalDamage))
		return;
}
