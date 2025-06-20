// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "DataContainer/SaveData.h"
#include "SaveManager.generated.h"

/**
 * 
 */
UCLASS()
class PACHIO_API USaveManager : public UObject
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Save")
    static void SaveToJson(const FStageSaveData& Data);

    UFUNCTION(BlueprintCallable, Category = "Save")
    static FStageSaveData LoadFromJson();
};
