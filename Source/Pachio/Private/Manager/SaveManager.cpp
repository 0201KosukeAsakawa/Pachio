// Fill out your copyright notice in the Description page of Project Settings.


#include "Manager/SaveManager.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Serialization/JsonWriter.h"
#include "Serialization/JsonSerializer.h"
#include "DataContainer/SaveData.h"

// =======================
// ステージデータの保存
// =======================

// ステージ単体データを保存
// @param StageKey - ステージ名（ユニークキー）
// @param NewData  - 保存するステージデータ
void USaveManager::SaveStageData(const FString& StageKey, FSaveData NewData)
{
    // 既存データを読み込み
    FStageSaveData CurrentData = LoadFromJson();

    // 指定キーに新データを上書きまたは追加
    CurrentData.Stages.FindOrAdd(StageKey) = NewData;

    // JSON ファイルに書き出し
    SaveToJson(CurrentData);
}

// ステージデータを JSON ファイルに保存
void USaveManager::SaveToJson(const FStageSaveData& InData)
{
    FString SavePath = FPaths::ProjectSavedDir() + "StageSave.json";
    FStageSaveData Data = InData;

    // ファイルが存在しなければ警告ログ
    if (!FPaths::FileExists(SavePath))
    {
        UE_LOG(LogTemp, Warning, TEXT("Save file not found. Creating default."));
    }

    // JSON にシリアライズして保存
    FString OutputString;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
    FJsonSerializer::Serialize(Data.ToJson().ToSharedRef(), Writer);
    FFileHelper::SaveStringToFile(OutputString, *SavePath);
}

// ステージデータを JSON ファイルから読み込み
FStageSaveData USaveManager::LoadFromJson()
{
    FString SavePath = FPaths::ProjectSavedDir() + "StageSave.json";
    FString Input;
    FStageSaveData LoadedData;

    // ファイルが存在すれば読み込み
    if (FFileHelper::LoadFileToString(Input, *SavePath))
    {
        TSharedPtr<FJsonObject> Json;
        TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Input);

        // JSON をデシリアライズ
        if (FJsonSerializer::Deserialize(Reader, Json))
        {
            LoadedData = FStageSaveData::FromJson(Json);
        }
    }

    return LoadedData;
}

// 指定ステージのクリアランクを取得
EStageRank USaveManager::GetStageRank(const FString& StageKey)
{
    FStageSaveData Data = LoadFromJson();
    return Data.GetStageRank(StageKey);
}

// =======================
// 音量データの保存・読み込み
// =======================

// 音量データを JSON ファイルに保存
void USaveManager::SaveVolumeToJson(const FVolumeSaveData& InData)
{
    FString SavePath = FPaths::ProjectSavedDir() + "VolumeSave.json";

    FString OutputString;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
    FJsonSerializer::Serialize(InData.ToJson().ToSharedRef(), Writer);
    FFileHelper::SaveStringToFile(OutputString, *SavePath);
}

// 音量データを JSON ファイルから読み込み
FVolumeSaveData USaveManager::LoadVolumeFromJson()
{
    FString SavePath = FPaths::ProjectSavedDir() + "VolumeSave.json";
    FString Input;
    FVolumeSaveData LoadedData;

    // ファイルが存在すれば読み込み
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
        // ファイルが存在しない場合はデフォルト作成
        UE_LOG(LogTemp, Warning, TEXT("Volume save file not found, creating new with default values."));
        SaveVolumeToJson(LoadedData);
    }

    return LoadedData;
}

// =======================
// 音量データの取得・設定
// =======================

// BGM 音量を取得
float USaveManager::GetBGMVolume()
{
    return LoadVolumeFromJson().BGMVolume;
}

// SE 音量を取得
float USaveManager::GetSEVolume()
{
    return LoadVolumeFromJson().SEVolume;
}

// 音量を保存（BGM/SE 両方）
void USaveManager::SetVolume(float NewBGM, float NewSE)
{
    FVolumeSaveData VolumeData;
    VolumeData.BGMVolume = NewBGM;
    VolumeData.SEVolume = NewSE;

    // JSON に保存
    SaveVolumeToJson(VolumeData);
}
