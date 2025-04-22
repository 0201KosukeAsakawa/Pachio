// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Attack/AttackStrategy.h"

#include "IDamageable.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UDamageable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PACHIO_API IDamageable
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	//ƒ_ƒ[ƒW‚ğó‚¯‚éˆ—
	virtual bool TakeDamage(FAttackData, float damage = 0);
	//€‚ñ‚¾‚©
	virtual bool IsDead()const;
};
