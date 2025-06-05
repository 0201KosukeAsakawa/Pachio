// Fill out your copyright notice in the Description page of Project Settings.


#include "Interface/AttackController.h"

// Add default functionality here for any IAttackController functions that are not pure virtual.

bool IAttackController::AssignAttackStrategy(FName AttackID, UAttackStrategy* NewStrategy)
{
	return false;
}
