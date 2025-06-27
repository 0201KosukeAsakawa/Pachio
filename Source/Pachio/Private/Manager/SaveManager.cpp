// Fill out your copyright notice in the Description page of Project Settings.


#include "Manager/SaveManager.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Serialization/JsonWriter.h"
#include "Serialization/JsonSerializer.h"
#include "DataContainer/SaveData.h"

void USaveManager::SaveStageData(const FString& StageKey, FSaveData NewData)
{
    FStageSaveData CurrentData = LoadFromJson();

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