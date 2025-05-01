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

USTRUCT(BlueprintType)
struct FInstansBlockData : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	FString MaterialID;
	UPROPERTY(EditAnywhere)
	FString StateID;
	UPROPERTY(EditAnywhere)
	FString DropItem;
	UPROPERTY(EditAnywhere)
	float Location_X;
	UPROPERTY(EditAnywhere)
	float Location_Y;
	UPROPERTY(EditAnywhere)
	float Location_Z;
	UPROPERTY(EditAnywhere)
	float Rotate_X;
	UPROPERTY(EditAnywhere)
	float Rotate_Y;
	UPROPERTY(EditAnywhere)
	float Rotate_Z;
	UPROPERTY(EditAnywhere)
	float Scale_X;
	UPROPERTY(EditAnywhere)
	float Scale_Y;
	UPROPERTY(EditAnywhere)
	float Scale_Z;
};


UCLASS(Blueprintable)
class PACHIO_API UBlockDataContainer : public UObject
{
    GENERATED_BODY()
public:

    UBlockState* CreateState(UObject*, FString) const;
    UMaterialInterface* CreateMaterial(UObject* WorldContext, FString StateName);
    bool GenerateBlock(FString, FString, FString, FVector, FVector,FRotator);

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data")
    TMap<FString, TSubclassOf<UBlockState>> BlockClassMap;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data")
    TMap<FString, TSoftObjectPtr<UMaterialInterface>> MaterialMap;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data")
    TSubclassOf<ABaseBlock> BlockClass;
};
