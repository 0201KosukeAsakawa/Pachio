// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EffectMatchResult.generated.h"

UENUM(BlueprintType)
enum class EBuffEffect : uint8
{
    None        UMETA(DisplayName = "None"),
    JumpBoost   UMETA(DisplayName = "Jump Boost"),
    SpeedBoost  UMETA(DisplayName = "Speed Boost"),
    Shield      UMETA(DisplayName = "Shield")
};



USTRUCT(BlueprintType)
struct FEffectMatchResult
{
    GENERATED_BODY()

    // 最も近いバフ効果
    UPROPERTY(BlueprintReadOnly)
    EBuffEffect ClosestEffect = EBuffEffect::None;

    // 色相距離
    UPROPERTY(BlueprintReadOnly)
    float Distance = 9999.0f;

    // 強度（0.0 ～ 1.0）
    UPROPERTY(BlueprintReadOnly)
    float StrengthRatio = 0.0f;
};