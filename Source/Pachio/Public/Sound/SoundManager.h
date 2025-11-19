// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Sound/SoundWave.h"
#include "Components/AudioComponent.h"
#include "Components/ActorComponent.h"
#include "Interface/Soundable.h"
#include "DataContainer/ColorTargetTypes.h"
#include "fmod_studio.hpp"     // FMOD Studio APIのC++ラッパー
#include "SoundManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBeatDetected);  
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnConfirmedBeat);  // マーカーで受けた正確なビート

class UFMODAudioComponent;
class UFMODEvent;
// サウンドデータを格納する構造体
USTRUCT()
struct FSoundData : public FTableRowBase
{
    GENERATED_USTRUCT_BODY()
public:
    // 再生対象の音（SoundWave or SoundCue）を保持
    UPROPERTY(EditAnywhere, Category = "Sound")
    TMap<FName, USoundBase*> SoundAssetMap;

    // AudioComponent（再生時に生成される）を保持
    UPROPERTY(Transient)
    TMap<FName, UAudioComponent*> AudioComponentMap;
};

/**
 * @brief サウンド管理コンポーネント
 * BGM/SEの再生、音量管理、フェードイン/アウト、Beat連動処理を担当
 */
UCLASS(ClassGroup = (Custom), Blueprintable, meta = (BlueprintSpawnableComponent))
class PACHIO_API USoundManager : public UActorComponent, public ISoundable
{
    GENERATED_BODY()

    friend class ALevelManager;

public:
    /** @brief コンストラクタ：デフォルト値設定 */
    USoundManager();

    /** @brief サウンドマネージャー初期化処理 */
    void Init();

    /**
     * @brief 一時的な色データ設定（Beatに連動用）
     * @param Mode 設定対象のカラータイプ
     * @param NewColor 設定する色
     */
    UFUNCTION(Category = "Beat")
    void SetTmp(FLinearColor NewColor);

private:
    // ==========================
    // ==== ボリューム管理関数 ===
    // ==========================

    /** @brief ボリューム設定のロードまたは新規作成 */
    void LoadOrCreateVolumeSave();

    /**
     * @brief BGM/SE音量を設定
     * @param NewBGM BGM音量（0-1）
     * @param NewSE SE音量（0-1）
     */
    UFUNCTION(BlueprintCallable)
    void SetVolume(float NewBGM, float NewSE);

    /**
     * @brief BGM音量設定
     * @param vol 音量（0-1）
     */
    UFUNCTION(BlueprintCallable)
    void SetBGMVolume(float vol) override;

    /**
     * @brief SE音量設定
     * @param vol 音量（0-1）
     */
    UFUNCTION(BlueprintCallable)
    virtual void SetSEVolume(float vol);

    /** @brief BGM停止 */
    void StopBGM() override;

    /** @brief 現在のBGM音量取得 */
    UFUNCTION(BlueprintCallable)
    float GetBGMVolume() const override { return BGMVolume; }

    /** @brief 現在のSE音量取得 */
    UFUNCTION(BlueprintCallable)
    float GetSEVolume() const override { return SEVolume; }

    // ==========================
    // ==== サウンド再生関数 ====
    // ==========================

    /**
     * @brief サウンド再生
     * @param DataID サウンドデータID
     * @param SoundID サウンドID
     * @param SetVolume 音量設定を反映するか
     * @param Volume 音量（0-1）
     * @param IsSpecifyLocation 指定位置で再生するか
     * @param place 再生位置（IsSpecifyLocation=true時に使用）
     * @return bool 再生成功か否か
     */
    UFUNCTION(BlueprintCallable)
    bool PlaySound(FName DataID, FName SoundID, const bool SetVolume = false, float Volume = 1, bool IsSpecifyLocation = false, FVector place = FVector(0.0f, 0.0f, 0.0f)) override;

    /**
     * @brief サウンド再生（フェードイン付き）
     * @param DataID サウンドデータID
     * @param SoundID サウンドID
     * @param Volume 音量
     * @param FadeDuration フェードイン時間
     */
    void PlaySoundWithFadeIn(FName DataID, FName SoundID, float Volume, float FadeDuration) override;

    /**
     * @brief BGM停止（フェードアウト付き）
     * @param FadeDuration フェードアウト時間
     */
    void StopBGMWithFadeOut(float FadeDuration) override;

    /**
     * @brief Envelope（音量包絡）値取得コールバック
     * @param SoundWave 対象SoundWave
     * @param EnvelopeValue 現在の音量包絡値
     */
    UFUNCTION()
    void OnEnvelopeValue(const USoundWave* SoundWave, const float EnvelopeValue);

    /** @brief BGM再生開始 */
    bool PlayBGM();

private:
    // ==========================
    // ==== サウンドデータ ======
    // ==========================

    /** @brief サウンドデータ保持マップ
     * Key: データID, Value: FSoundData
     */
    UPROPERTY(EditAnywhere, Category = "Sound")
    TMap<FName, FSoundData> SoundDataMap;

    // ==========================
    // ==== BGM関連コンポーネント ====
    // ==========================

    /** @brief 現在再生中のBGM AudioComponent */
    UPROPERTY()
    UFMODAudioComponent* mCurrentBGM;

    /** @brief BGM用AudioComponent */
    UPROPERTY()
    UFMODAudioComponent* BGM;

    /** @brief FMOD EventAsset（BGM用） */
    UPROPERTY(EditAnywhere, Category = "FMOD")
    UFMODEvent* BGMEventAsset;

    /** @brief FMOD EventInstance */
    FMOD::Studio::EventInstance* EventInstance;

    /** @brief FMOD AudioComponent参照 */
    UPROPERTY()
    UFMODAudioComponent* FMODAudioComponent;

    /** @brief BGM音量 */
    float BGMVolume;

    /** @brief SE音量 */
    float SEVolume;

    // ==========================
    // ==== BPM/Beat管理 ========
    // ==========================

    /** @brief BPM値 */
    UPROPERTY(EditAnywhere, Category = "BPM")
    float MusicBPM;

    /** @brief Beat発火用タイマー */
    FTimerHandle BeatTimerHandle;

    /** @brief Beat間隔（秒） */
    float BeatInterval;

    /** @brief 再生開始時間 */
    float StartTime;

    /** @brief 最後に予測したBeat */
    int32 LastPredictedBeat;

    /** @brief 最後に確認したBeat時間 */
    float LastConfirmedBeatTime;

    /** @brief Beat確定時イベント */
    UPROPERTY(BlueprintAssignable)
    FOnConfirmedBeat OnConfirmedBeat;
};
