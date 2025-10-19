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

    /**
     * @brief 指定したステージのセーブデータを保存する。
     *
     * @param StageKey 対象ステージのキー（ID文字列）。
     * @param NewData 保存するステージデータ。
     */
    UFUNCTION(BlueprintCallable, Category = "Save")
    static void SaveStageData(const FString& StageKey, FSaveData NewData);

    /**
     * @brief ステージセーブデータを JSON 形式で保存する。
     *
     * @param Data 保存対象のステージセーブデータ構造体。
     */
    UFUNCTION(BlueprintCallable, Category = "Save")
    static void SaveToJson(const FStageSaveData& Data);

    /**
     * @brief JSON 形式のステージセーブデータをロードする。
     *
     * @return 読み込んだステージセーブデータ。
     */
    UFUNCTION(BlueprintCallable, Category = "Save")
    static FStageSaveData LoadFromJson();

    /**
     * @brief 指定ステージのクリアランクを取得する。
     *
     * @param StageKey 対象ステージのキー。
     * @return ステージのクリアランク（EStageRank）。
     */
    UFUNCTION(BlueprintCallable, Category = "Save")
    EStageRank GetStageRank(const FString& StageKey);

    /**
     * @brief ボリュームデータを JSON 形式で保存する。
     *
     * @param InData 保存対象のボリューム設定データ。
     */
    UFUNCTION(BlueprintCallable, Category = "Save")
    static void SaveVolumeToJson(const FVolumeSaveData& InData);

    /**
     * @brief JSON 形式のボリューム設定データをロードする。
     *
     * @return 読み込んだボリューム設定データ。
     */
    UFUNCTION(BlueprintCallable, Category = "Save")
    static FVolumeSaveData LoadVolumeFromJson();

    /**
     * @brief 現在の BGM 音量を取得する。
     *
     * @return BGM 音量（0.0f ～ 1.0f）。
     */
    UFUNCTION(BlueprintCallable, Category = "Save")
    static float GetBGMVolume();

    /**
     * @brief 現在の SE 音量を取得する。
     *
     * @return SE 音量（0.0f ～ 1.0f）。
     */
    UFUNCTION(BlueprintCallable, Category = "Save")
    static float GetSEVolume();

    /**
     * @brief BGM と SE の音量を設定する。
     *
     * @param NewBGM 設定する BGM 音量（0.0f ～ 1.0f）。
     * @param NewSE 設定する SE 音量（0.0f ～ 1.0f）。
     */
    UFUNCTION(BlueprintCallable, Category = "Save")
    static void SetVolume(float NewBGM, float NewSE);
};
