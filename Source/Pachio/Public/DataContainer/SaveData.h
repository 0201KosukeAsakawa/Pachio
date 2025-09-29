// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DataContainer/StageInfo.h"
#include "SaveData.generated.h"

USTRUCT(BlueprintType)
struct FSaveData
{
    GENERATED_BODY()

    bool bCleared = false;
    EStageRank ClearRank;
    int32 difficultyRank = 0;
    FString Title;

    TSharedPtr<FJsonObject> ToJson() const
    {
        TSharedPtr<FJsonObject> Obj = MakeShareable(new FJsonObject);
        Obj->SetBoolField("Cleared", bCleared);
        Obj->SetNumberField("Rank", static_cast<int32>(ClearRank));
        Obj->SetNumberField("DifficultyRank", difficultyRank);
        Obj->SetStringField("Title", Title);
        return Obj;
    }

    static FSaveData FromJson(TSharedPtr<FJsonObject> Obj)
    {
        FSaveData Data;
        Data.bCleared = Obj->GetBoolField("Cleared");
        Data.ClearRank = static_cast<EStageRank>(Obj->GetIntegerField("Rank"));
        Data.difficultyRank = Obj->GetNumberField("DifficultyRank");
        Data.Title = Obj->GetStringField("Title");
        return Data;
    }
};

USTRUCT(BlueprintType)
struct FStageSaveData
{

    GENERATED_BODY()
    TMap<FString, FSaveData> Stages;

    TSharedPtr<FJsonObject> ToJson() const
    {
        TSharedPtr<FJsonObject> Root = MakeShareable(new FJsonObject);
        TSharedPtr<FJsonObject> StageObject = MakeShareable(new FJsonObject);

        for (const auto& Elem : Stages)
        {
            StageObject->SetObjectField(Elem.Key, Elem.Value.ToJson());
        }

        Root->SetObjectField("Stages", StageObject);
        return Root;
    }

    static FStageSaveData FromJson(TSharedPtr<FJsonObject> Root)
    {
        FStageSaveData Data;
        const TSharedPtr<FJsonObject> StageObject = Root->GetObjectField("Stages");

        for (auto& Pair : StageObject->Values)
        {
            const FString& StageName = Pair.Key;
            const TSharedPtr<FJsonObject> StageDataJson = Pair.Value->AsObject();

            FSaveData StageData = FSaveData::FromJson(StageDataJson);
            Data.Stages.Add(StageName, StageData);
        }

        return Data;
    }

    EStageRank GetStageRank(const FString& StageName) const
    {
        if (const FSaveData* FoundData = Stages.Find(StageName))
        {
            return FoundData->ClearRank;
        }
        return EStageRank::None;
    }
};


USTRUCT(BlueprintType)
struct FVolumeSaveData
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite)
    float BGMVolume = 2;

    UPROPERTY(BlueprintReadWrite)
    float SEVolume = 2;

    TSharedPtr<FJsonObject> ToJson() const
    {
        TSharedPtr<FJsonObject> Obj = MakeShareable(new FJsonObject);
        Obj->SetNumberField("BGMVolume", BGMVolume);
        Obj->SetNumberField("SEVolume", SEVolume);
        return Obj;
    }

    static FVolumeSaveData FromJson(const TSharedPtr<FJsonObject>& Obj)
    {
        FVolumeSaveData Data;
        Data.BGMVolume = Obj->GetNumberField("BGMVolume");
        Data.SEVolume = Obj->GetNumberField("SEVolume");
        return Data;
    }
};
