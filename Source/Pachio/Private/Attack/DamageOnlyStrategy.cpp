// Fill out your copyright notice in the Description page of Project Settings.


#include "Attack/DamageOnlyStrategy.h"
#include "Interface/IDamageable.h"

void UDamageOnlyStrategy::ExecuteEffect(AActor* Attacker, AActor* Target, FAttackData attackData, float FinalDamage)
{
	IDamageable* id = Cast<IDamageable>(Target);
	if (!id)
		return;

	id->TakeDamage(attackData, FinalDamage);

}
