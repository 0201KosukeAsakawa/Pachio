// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "BlockDataContainer.generated.h"

class UBlockState;
/**
 * 
 */
UCLASS(Blueprintable)
class PACHIO_API UBlockDataContainer : public UObject
{
	GENERATED_BODY()
public:

    // blockのステートを保持するデータコンテナ
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack")
    TMap<FString, TSubclassOf<UBlockState>> AttackStrategyMap;

    // ステートをインスタンス化して返す
    UBlockState* CreateState(UObject*, FString) const;
};
