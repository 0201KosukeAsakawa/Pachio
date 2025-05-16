// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "EnemyDataContainer.generated.h"

class AEnemyCharacter;
class UEnemyStateComponent;

USTRUCT()
struct FMaterialData : public FTableRowBase
{
    GENERATED_USTRUCT_BODY()

public:
    UPROPERTY(EditAnywhere, Category = "MaterialType")
    TMap<FString, TSoftObjectPtr<UMaterialInterface>> material;
};

UCLASS(Blueprintable)
class PACHIO_API UEnemyDataContainer : public UObject
{
	GENERATED_BODY()
public:

    UEnemyStateComponent* CreateState(UObject*, FString) const;
    UMaterialInterface* CreateMaterial(UObject* WorldContext, FString StateName ,const FString = "Default");
    bool GenerateEnemy(FString stateID, FString dropItemID, FString materialID, FVector location, FVector scale, FRotator rotator);


private:
    UPROPERTY(EditAnywhere, Category = "Data")
    TMap<FString, TSubclassOf<UEnemyStateComponent>> EnemyClassMap;

    UPROPERTY(EditAnywhere, Category = "Data")
    TMap<FString, FMaterialData> MaterialMap;

    UPROPERTY(EditAnywhere, Category = "Data")
    TSubclassOf<AEnemyCharacter> EnemyCharacter;
};
