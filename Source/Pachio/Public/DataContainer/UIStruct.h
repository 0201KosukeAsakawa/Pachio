// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UIStruct.generated.h"

UENUM(BlueprintType)
enum class EWidgetCategory : uint8
{
    Menu     UMETA(DisplayName = "Menu"),
    Tutorial UMETA(DisplayName = "Tutorial"),
    Lens     UMETA(DisplayName = "Lens"),
    Score    UMETA(DisplayName = "Score"),
    Marker   UMETA(DisplayName = "Marker")
};

/**
 * 複数ウィジェットカテゴリ（例: State, Menu, HUD）に対応するデータ構造
 */
USTRUCT(BlueprintType)
struct FWidgetData : public FTableRowBase
{
    GENERATED_USTRUCT_BODY()

    // ウィジェット名に対応するウィジェットクラス
    UPROPERTY(EditAnywhere, Category = "UI")
    TMap<FName, TSubclassOf<UUserWidget>> WidgetClassMap;

    // 実行時に生成されたウィジェットのインスタンスを保持
    UPROPERTY(Transient)
    TMap<FName, UUserWidget*> WidgetMap;

    // 現在表示中のウィジェット
    UPROPERTY(Transient)
    TMap<FName, UUserWidget*> CurrentWidget;
};

