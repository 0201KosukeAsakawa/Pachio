// Fill out your copyright notice in the Description page of Project Settings.


#include "Interface/IDamageable.h"

// Add default functionality here for any IIDamageable functions that are not pure virtual.

bool IDamageable::TakeDamage(FAttackData, float)
{
    return false;
}

bool IDamageable::IsDead() const
{
    return false;
}
