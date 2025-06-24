// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SaveData.generated.h"

USTRUCT(BlueprintType)
struct FSaveData
{
    GENERATED_BODY()

    bool bCleared = false;
    int32 Score = 0;
    float Time = 0.0f;

    TSharedPtr<FJsonObject> ToJson() const
    {
        TSharedPtr<FJsonObject> Obj = MakeShareable(new FJsonObject);
        Obj->SetBoolField("Cleared", bCleared);
        Obj->SetNumberField("Score", Score);
        Obj->SetNumberField("Time", Time);
        return Obj;
    }

    static FSaveData FromJson(TSharedPtr<FJsonObject> Obj)
    {
        FSaveData Data;
        Data.bCleared = Obj->GetBoolField("Cleared");
        Data.Score = Obj->GetIntegerField("Score");
        Data.Time = Obj->GetNumberField("Time");
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
};