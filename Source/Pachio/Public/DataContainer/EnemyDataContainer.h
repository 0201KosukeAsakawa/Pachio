// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Enemy/EnemyCategory.h"
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

    UEnemyStateComponent* CreateState(UObject*, EEnemyCategory) const;
    UMaterialInterface* CreateMaterial(UObject* WorldContext, EEnemyCategory StateName ,const FString = "Default");
    bool GenerateEnemy(EEnemyCategory stateID, FString dropItemID, EEnemyCategory materialID, FVector location, FVector scale, FRotator rotator);


private:


    UPROPERTY(EditAnywhere, Category = "Data")
    TMap<EEnemyCategory,FMaterialData> MaterialMap;

    UPROPERTY(EditAnywhere, Category = "Data")
    TSubclassOf<AEnemyCharacter> EnemyCharacter;
};
