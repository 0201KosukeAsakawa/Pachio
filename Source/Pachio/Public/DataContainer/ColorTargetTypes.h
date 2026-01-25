// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ColorTargetTypes.generated.h"

class IColorReactive;

namespace
{
    constexpr float COLOR_CATEGORIES = 14;
}

UENUM(BlueprintType)
enum class EColorCategory : uint8
{
    Black        UMETA(DisplayName = "BLACK"),
    White        UMETA(DisplayName = "White"),
    Red             UMETA(DisplayName = "RED"),
    Orange          UMETA(DisplayName = "ORANGE"),
    Yellow         UMETA(DisplayName = "YELLOW"),
    Chatreuse      UMETA(DisplayName = "CHARTREUSE"),
    Green          UMETA(DisplayName = "GREEEEN"),
    SpringGreen    UMETA(DisplayName = "SPRING_GREEN"),
    Cyan         UMETA(DisplayName = "CYAN"),
    Azure         UMETA(DisplayName = "AZURE"),
    Blue          UMETA(DisplayName = "BLUE"),
    Violet        UMETA(DisplayName = "VIOLET"),
    Magenta        UMETA(DisplayName = "MAGENTA"),
    Rose         UMETA(DisplayName = "ROSE"),
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