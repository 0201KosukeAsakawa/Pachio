// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Sound/SoundWave.h"
#include "Components/AudioComponent.h"
#include "Components/ActorComponent.h"
#include "Interface/Soundable.h"
#include "SoundManager.generated.h"


// サウンドデータを格納する構造体
USTRUCT()
struct FSoundData : public FTableRowBase
{
    GENERATED_USTRUCT_BODY()

public:
    // サウンドウェーブとその対応するオーディオコンポーネントを保持
    UPROPERTY(EditAnywhere, Category = "Sound")
    TMap<FName, USoundWave*> SoundWaveMap;

    UPROPERTY()
    TMap<FName, UAudioComponent*> AudioComponentMap;
};


UCLASS( ClassGroup=(Custom), Blueprintable, meta=(BlueprintSpawnableComponent) )
class PACHIO_API USoundManager : public UObject, public ISoundable
{
	GENERATED_BODY()

    friend class AInGameSoundManager;

public:	
	// Sets default values for this component's properties
	USoundManager();

    // サウンドマネージャーを初期化
    void Init();

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

private:
    // サウンドデータを保持するためのマップ
    UPROPERTY(EditAnywhere, Category = "Sound")
    TMap<FName, FSoundData> SoundDataMap;

    // 現在再生中のBGM
    UPROPERTY()
    UAudioComponent* mCurrentBGM;

    // BGM音量
    float BGMVolume;

    // SE音量
    float SEVolume;

};