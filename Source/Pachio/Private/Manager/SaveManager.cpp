// Fill out your copyright notice in the Description page of Project Settings.


#include "Manager/SaveManager.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Serialization/JsonWriter.h"
#include "Serialization/JsonSerializer.h"
#include "DataContainer/SaveData.h"

FStageSaveData CreateDefaultStageData()
{
    FStageSaveData DefaultData;

    // ステージ1?5を未クリア状態で登録
    for (int32 i = 1; i <= 5; ++i)
    {
        FString StageKey = FString::Printf(TEXT("Stage%d"), i);
        FSaveData InitStage;
        InitStage.bCleared = false;
        InitStage.Score = 0;
        InitStage.Time = 0.0f;
        DefaultData.Stages.Add(StageKey, InitStage);
    }

    return DefaultData;
}

void USaveManager::SaveStageData(const FString& StageKey, FSaveData NewData)
{
    FStageSaveData CurrentData = LoadFromJson();

    if (CurrentData.Stages.Num() == 0)
    {
        CurrentData = CreateDefaultStageData();
    }

    CurrentData.Stages.FindOrAdd(StageKey) = NewData;

    SaveToJson(CurrentData);
}

void USaveManager::SaveToJson(const FStageSaveData& InData)
{
    FString SavePath = FPaths::ProjectSavedDir() + "StageSave.json";
    FStageSaveData Data = InData;

    if (!FPaths::FileExists(SavePath))
    {
        UE_LOG(LogTemp, Warning, TEXT("Save file not found. Creating default."));
        Data = CreateDefaultStageData();
    }

    FString OutputString;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
    FJsonSerializer::Serialize(Data.ToJson().ToSharedRef(), Writer);
    FFileHelper::SaveStringToFile(OutputString, *SavePath);
}

FStageSaveData USaveManager::LoadFromJson()
{
    FString SavePath = FPaths::ProjectSavedDir() + "StageSave.json";
    FString Input;
    FStageSaveData LoadedData;

    if (FFileHelper::LoadFileToString(Input, *SavePath))
    {
        TSharedPtr<FJsonObject> Json;
        TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Input);
        if (FJsonSerializer::Deserialize(Reader, Json))
        {
            LoadedData = FStageSaveData::FromJson(Json);
        }
    }

    return LoadedData;
}