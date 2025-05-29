// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyCategory.generated.h"

UENUM(BlueprintType)
enum class EEnemyCategory : uint8
{
    None UMETA(DisplayName = "None"),
    Goomba UMETA(DisplayName = "Goomba"),
    Koopa UMETA(DisplayName = "Koopa"),
};