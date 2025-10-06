// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DataContainer/StageInfo.h"
#include "SaveData.generated.h"

// =======================
// ステージごとのセーブデータ
// =======================

USTRUCT(BlueprintType)
struct FSaveData
{
    GENERATED_BODY()

    // ステージクリア済みかどうか
    bool bCleared = false;

    // クリアランク（例：S, A, B…）
    EStageRank ClearRank;

    // 難易度ランク（数値）
    int32 difficultyRank = 0;

    // ステージタイトル
    FString Title;

    // JSON 形式に変換
    TSharedPtr<FJsonObject> ToJson() const
    {
        TSharedPtr<FJsonObject> Obj = MakeShareable(new FJsonObject);
        Obj->SetBoolField("Cleared", bCleared);
        Obj->SetNumberField("Rank", static_cast<int32>(ClearRank));
        Obj->SetNumberField("DifficultyRank", difficultyRank);
        Obj->SetStringField("Title", Title);
        return Obj;
    }

    // JSON から構造体に復元
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

// =======================
// 全ステージのセーブデータ
// =======================

USTRUCT(BlueprintType)
struct FStageSaveData
{
    GENERATED_BODY()

    // ステージ名をキーにしたセーブデータのマップ
    TMap<FString, FSaveData> Stages;

    // JSON に変換
    TSharedPtr<FJsonObject> ToJson() const
    {
        TSharedPtr<FJsonObject> Root = MakeShareable(new FJsonObject);
        TSharedPtr<FJsonObject> StageObject = MakeShareable(new FJsonObject);

        // 各ステージのデータを JSON に追加
        for (const auto& Elem : Stages)
        {
            StageObject->SetObjectField(Elem.Key, Elem.Value.ToJson());
        }

        Root->SetObjectField("Stages", StageObject);
        return Root;
    }

    // JSON から全ステージデータを復元
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

    // 指定ステージのランクを取得（存在しない場合は None）
    EStageRank GetStageRank(const FString& StageName) const
    {
        if (const FSaveData* FoundData = Stages.Find(StageName))
        {
            return FoundData->ClearRank;
        }
        return EStageRank::None;
    }
};

// =======================
// 音量設定のセーブデータ
// =======================

USTRUCT(BlueprintType)
struct FVolumeSaveData
{
    GENERATED_BODY()

    // BGM 音量
    UPROPERTY(BlueprintReadWrite)
    float BGMVolume = 2;

    // 効果音(SE) 音量
    UPROPERTY(BlueprintReadWrite)
    float SEVolume = 2;

    // JSON に変換
    TSharedPtr<FJsonObject> ToJson() const
    {
        TSharedPtr<FJsonObject> Obj = MakeShareable(new FJsonObject);
        Obj->SetNumberField("BGMVolume", BGMVolume);
        Obj->SetNumberField("SEVolume", SEVolume);
        return Obj;
    }

    // JSON から復元
    static FVolumeSaveData FromJson(const TSharedPtr<FJsonObject>& Obj)
    {
        FVolumeSaveData Data;
        Data.BGMVolume = Obj->GetNumberField("BGMVolume");
        Data.SEVolume = Obj->GetNumberField("SEVolume");
        return Data;
    }
};
