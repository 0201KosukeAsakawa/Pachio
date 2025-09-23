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
    static
    void SaveStageData(const FString& StageKey, FSaveData NewData);
    UFUNCTION(BlueprintCallable, Category = "Save")
    static void SaveToJson(const FStageSaveData& Data);

    UFUNCTION(BlueprintCallable, Category = "Save")
    static FStageSaveData LoadFromJson();

    UFUNCTION(BlueprintCallable, Category = "Save")
    EStageRank GetStageRank(const FString& StageKey);
    
    UFUNCTION(BlueprintCallable, Category = "Save")
    static void SaveVolumeToJson(const FVolumeSaveData& InData);

    UFUNCTION(BlueprintCallable, Category = "Save")
    static FVolumeSaveData LoadVolumeFromJson();

    UFUNCTION(BlueprintCallable, Category = "Save")
    static float GetBGMVolume();

    UFUNCTION(BlueprintCallable, Category = "Save")
    static float GetSEVolume();
    
    UFUNCTION(BlueprintCallable, Category = "Save")
    static void SetVolume(float NewBGM, float NewSE);
};
