// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ClearScoreType.generated.h"

UENUM(BlueprintType)
enum class EClearScore : uint8
{
	S UMETA(DisplayName = "SRANK"),
	A UMETA(DisplayName = "ARANK"),
	B UMETA(DisplayName = "BRANK"),
};

