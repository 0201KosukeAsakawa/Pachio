// Fill out your copyright notice in the Description page of Project Settings.


#include "Attack/AttackStrategy.h"

UAttackStrategy::UAttackStrategy()
{
}

bool UAttackStrategy::ExecuteEffect(AActor* Attacker, AActor* Target, FAttackData, float FinalDamage)
{
	return false;
}
