// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "BlockDataContainer.generated.h"

class UBlockState;
class ABaseBlock;
/**
 * 
 */
UCLASS(Blueprintable)
class PACHIO_API UBlockDataContainer : public UObject
{
	GENERATED_BODY()
public:

    UBlockState* CreateState(UObject*, FString) const;
    UMaterialInterface* CreateMaterial(UObject* WorldContext, FString StateName);
    bool GenerateBlock(FString,FString,FString,FVector,FRotator);

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data")
    TMap<FString, TSubclassOf<UBlockState>> AttackStrategyMap;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data")
    TMap<FString, TSoftObjectPtr<UMaterialInterface>> MaterialMap;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data")
    TSubclassOf<ABaseBlock> BlockClass;
};
