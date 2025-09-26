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

EStageRank USaveManager::GetStageRank(const FString& StageKey)
{
    FStageSaveData Data = LoadFromJson();
    return Data.GetStageRank(StageKey);
}


void USaveManager::SaveVolumeToJson(const FVolumeSaveData& InData)
{
    FString SavePath = FPaths::ProjectSavedDir() + "VolumeSave.json";

    FString OutputString;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
    FJsonSerializer::Serialize(InData.ToJson().ToSharedRef(), Writer);
    FFileHelper::SaveStringToFile(OutputString, *SavePath);
}

FVolumeSaveData USaveManager::LoadVolumeFromJson()
{
    FString SavePath = FPaths::ProjectSavedDir() + "VolumeSave.json";
    FString Input;
    FVolumeSaveData LoadedData;

    if (FFileHelper::LoadFileToString(Input, *SavePath))
    {
        TSharedPtr<FJsonObject> Json;
        TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Input);
        if (FJsonSerializer::Deserialize(Reader, Json))
        {
            LoadedData = FVolumeSaveData::FromJson(Json);
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Volume save file not found, creating new with default values."));
        SaveVolumeToJson(LoadedData);
    }

    return LoadedData;
}

float USaveManager::GetBGMVolume()
{
    return LoadVolumeFromJson().BGMVolume;
}

float USaveManager::GetSEVolume()
{
    return LoadVolumeFromJson().SEVolume;
}

void USaveManager::SetVolume(float NewBGM, float NewSE)
{
    FVolumeSaveData VolumeData;
    VolumeData.BGMVolume = NewBGM;
    VolumeData.SEVolume = NewSE;
    SaveVolumeToJson(VolumeData);
}

