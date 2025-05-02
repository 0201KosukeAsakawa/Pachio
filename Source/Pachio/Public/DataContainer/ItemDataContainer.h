// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ItemDataContainer.generated.h"

class UItemEffectSourceComponent;
class AItemBase;

UCLASS(Blueprintable)
class PACHIO_API UItemDataContainer : public UObject
{
	GENERATED_BODY()
public:

    UItemEffectSourceComponent* CreateState(UObject*, FString) const;
    UMaterialInterface* CreateMaterial(UObject* WorldContext, FString StateName);
    AItemBase* GenerateItem(FString ItemID, FVector location, FVector direction , const float force = 0,const FVector addDirection = FVector(0,0,0), const FVector scale = FVector(1, 1, 1), const FRotator rotation = FRotator(0, 0, 0), const FString meshID = "None", const FString materialID = "None");

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data")
    TMap<FString, TSubclassOf<UItemEffectSourceComponent>> ItemClassMap;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data")
    TMap<FString, TSoftObjectPtr<UMaterialInterface>> MaterialMap;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data")
    TSubclassOf<AItemBase> ItemClass;
};
