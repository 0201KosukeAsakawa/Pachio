// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EffectMatchResult.generated.h"

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

// 色モードの対象を示す列挙型
UENUM(BlueprintType)
enum class EColorTargetType : uint8
{
    WorldColor      UMETA(DisplayName = "WorldColor"),       // レイヤー単位での色変更
    ObjectColor       UMETA(DisplayName = "Object"),      // オブジェクト単位での色変更
    Responders UMETA(DisplayName = "Responders"),   // 複数の反応対象（通知など）の色変更
    Event      UMETA(DisplayName = "Event"),
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

// 実体の色付け対象インスタンス群を格納する構造体
USTRUCT()
struct FColorTargetInstanceArray
{
    GENERATED_BODY()

    // 色付け対象インターフェースを持つインスタンス配列
    TArray<TScriptInterface<IColorReactiveInterface>> Instances;
};
