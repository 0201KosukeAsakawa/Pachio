// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ColorTargetTypes.generated.h"

class IColorReactiveInterface;

UENUM(BlueprintType)
enum class EColorCategory : uint8
{
    Black UMETA(DisplayName = "BLACK"),
    Red   UMETA(DisplayName = "RED"),
    Green  UMETA(DisplayName = "GREEEEN"),
    Blue      UMETA(DisplayName = "BLUE"),
    Yellow UMETA(DisplayName = "YELLOW"),
};

// 色付け対象のクラス群を格納する構造体（編集可能）
USTRUCT(BlueprintType)
struct FColorTargetArray
{
    GENERATED_BODY()

    // 色付け対象のクラス配列
    UPROPERTY(EditAnywhere)
    TArray<TSubclassOf<UObject>> Targets;
};