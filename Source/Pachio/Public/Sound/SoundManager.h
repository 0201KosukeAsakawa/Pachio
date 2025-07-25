// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Sound/SoundWave.h"
#include "Components/AudioComponent.h"
#include "Components/ActorComponent.h"
#include "Interface/Soundable.h"
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


UCLASS( ClassGroup=(Custom), Blueprintable, meta=(BlueprintSpawnableComponent) )
class PACHIO_API USoundManager : public UObject, public ISoundable
{
	GENERATED_BODY()

    friend class ALevelManager;

public:	
	// Sets default values for this component's properties
	USoundManager();

    // サウンドマネージャーを初期化
    void Init();
    void Tick(float DeltaTim);
private:
    // サウンドを再生するメソッド
    UFUNCTION(BlueprintCallable)
    bool PlaySound(FName DataID, FName SoundID, float Volume, bool IsSpecifyLocation = false, FVector place = FVector(0.0f, 0.0f, 0.0f))override;

    bool PlaySound(FName DataID, FName SoundID) override;
    UFUNCTION(BlueprintCallable)
    void SetSoundVolume(float BGMvol, float SEVol) override;
    void StopBGM(bool b) override;
    UFUNCTION(BlueprintCallable)
    float GetBGMVolume() const override { return BGMVolume; }
    UFUNCTION(BlueprintCallable)
    float GetSEVolume() const override { return SEVolume; }

    void PlaySoundWithFadeIn(FName DataID, FName SoundID, float Volume, float FadeDuration) override;
    void StopBGMWithFadeOut(float FadeDuration) override;
    UFUNCTION()
    void OnEnvelopeValue(const USoundWave* SoundWave, const float EnvelopeValue);
    bool PlayBGM();
private:
    // サウンドデータを保持するためのマップ
    UPROPERTY(EditAnywhere, Category = "Sound")
    TMap<FName, FSoundData> SoundDataMap;

    // 現在再生中のBGM
    UPROPERTY()
    UFMODAudioComponent* mCurrentBGM;

    // BGM音量
    float BGMVolume;

    // SE音量
    float SEVolume;

    UPROPERTY()
    UFMODAudioComponent* TestSound;
    UPROPERTY(EditAnywhere, Category = "FMOD")
    UFMODEvent* TestEventAsset;
    // SoundManager.h に追加
    UPROPERTY(EditAnywhere, Category = "BPM")
    float MusicBPM = 166.0f;  // 任意のBPM

    FTimerHandle BeatTimerHandle;// Beat発火処理
    UFUNCTION()
    void OnBeatTimerElapsed();
    void InitTestSound();

    float BeatInterval = 0.5f;
    float StartTime = 0.0f;
    int32 LastPredictedBeat = -1;
    float LastConfirmedBeatTime = 0.0f;
    UPROPERTY(BlueprintAssignable)
    FOnConfirmedBeat OnConfirmedBeat;
    UPROPERTY()
    class UFMODAudioComponent* FMODAudioComponent;
    FMOD::Studio::EventInstance* EventInstance;
public:
    UPROPERTY(BlueprintAssignable, Category = "Beat")
    FOnBeatDetected OnBeatDetected;
    void OnMarkerBeat(int64 MarkerPositionMs); // マーカーで発火されたとき
};