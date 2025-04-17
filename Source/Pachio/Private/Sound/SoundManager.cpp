#include "Sound/SoundManager.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"

USoundManager::USoundManager()
    : BGMVolume(0.5f)
    , SEVolume(1)
    ,mCurrentBGM(nullptr)
{
}

void USoundManager::Init()
{
    for (auto& soundMap : SoundDataMap)
    {
        const FName dataTag = soundMap.Key;
        FSoundData& soundData = soundMap.Value;
        soundData.AudioComponentMap.Reset();  // AudioComponentMapをリセット

        for (const auto& soundwaveMap : soundData.SoundWaveMap)
        {
            const FName waveTag = soundwaveMap.Key;
            USoundWave* soundWave = soundwaveMap.Value;
            if (waveTag.IsNone() || soundWave == nullptr)
                continue;

            // すでにAudioComponentが存在する場合はスキップ
            if (soundData.AudioComponentMap.Contains(waveTag))
                continue;

            // AudioComponentを作成してシーンにアタッチ
            UAudioComponent* AudioComponent = UGameplayStatics::CreateSound2D(this, soundWave);
            if (AudioComponent)
            {
                AudioComponent->bAutoDestroy = false;  // 自動削除を無効にする

                if (dataTag == "BGM")
                    soundWave->bLooping = true;

                // AudioComponentMapに追加
                soundData.AudioComponentMap.Add(waveTag, AudioComponent);
            }
        }
    }
}

void USoundManager::SetSoundVolume(float BGMvol, float SEVol)
{
    float previousBGMVolume = BGMVolume;

    // 音量を0から1の範囲に制限
    BGMVolume = FMath::Clamp(BGMvol, 0.0f, 1.0f);
    SEVolume = FMath::Clamp(SEVol, 0.0f, 1.0f);

    // BGM音量が変更されていた場合、音量変更後に再生を管理
    if (mCurrentBGM && previousBGMVolume != BGMVolume)
    {
        // 音量を設定
        mCurrentBGM->SetVolumeMultiplier(BGMVolume);

        // 音量が0でなく、かつBGMが停止している場合のみ再生
        if (BGMVolume > 0.0f && !mCurrentBGM->IsPlaying())
        {
            mCurrentBGM->Play(); // 音楽を再生
        }
        else if (BGMVolume == 0.0f)
        {
            mCurrentBGM->Stop();  // 音量が0なら停止
        }
    }
}

bool USoundManager::PlaySound(FName DataID, FName SoundID, float Volume, bool IsSpecifyLocation, FVector place)
{
    if (!SoundDataMap.Contains(DataID))
    {
        UE_LOG(LogTemp, Error, TEXT("SoundDataMap does not contain DataID: %s"), *DataID.ToString());
        return false;
    }

    if (!SoundDataMap[DataID].AudioComponentMap.Contains(SoundID))
    {
        UE_LOG(LogTemp, Error, TEXT("AudioComponentMap does not contain SoundID: %s for DataID: %s"), *SoundID.ToString(), *DataID.ToString());
        return false;
    }

    UAudioComponent* AudioComponent = SoundDataMap[DataID].AudioComponentMap[SoundID];
    if (!AudioComponent)
    {
        UE_LOG(LogTemp, Error, TEXT("AudioComponent is null for SoundID: %s in DataID: %s"), *SoundID.ToString(), *DataID.ToString());
        Init();
        return false;
    }

    // 音量を設定 (0.0が無音、1.0が最大音量)
    AudioComponent->SetVolumeMultiplier(FMath::Clamp(Volume, 0.0f, 1.0f));

    // 位置指定がある場合、音の位置を設定
    if (IsSpecifyLocation)
    {
        AudioComponent->SetWorldLocation(place);
    }

    // サウンドの距離減衰設定（Attenuation設定）
    FSoundAttenuationSettings AttenuationSettings;
    AttenuationSettings.bAttenuate = true; // 距離による減衰を有効にする
    AttenuationSettings.FalloffDistance = 2000.0f; // 音量が減衰する距離の設定（例：2000 units）

    // サウンドを再生
    AudioComponent->Play();

    // BGMが再生される場合、mCurrentBGMを更新
    if (DataID == "BGM")
    {
        mCurrentBGM = AudioComponent;
    }

    return true;
}

bool USoundManager::PlaySound(FName DataID, FName SoundID)
{
    float volume = 0;

    if (DataID == "BGM")
        volume = BGMVolume;
    else if (DataID == "SE")
        volume = SEVolume;

    return PlaySound(DataID, SoundID, volume);
}

void USoundManager::StopBGM(bool b)
{
    if (mCurrentBGM)
    {
        mCurrentBGM->Stop();
    }
}

void USoundManager::PlaySoundWithFadeIn(FName DataID, FName SoundID, float Volume, float FadeDuration)
{
    if (!SoundDataMap.Contains(DataID) || !SoundDataMap[DataID].AudioComponentMap.Contains(SoundID))
        return;

    UAudioComponent* AudioComponent = SoundDataMap[DataID].AudioComponentMap[SoundID];
    if (!AudioComponent)
        return;

    AudioComponent->SetVolumeMultiplier(FMath::Clamp(Volume, 0.0f, 1.0f));
    AudioComponent->FadeIn(FadeDuration);  // フェードインを追加
    AudioComponent->Play();
}

void USoundManager::StopBGMWithFadeOut(float FadeDuration)
{
    if (mCurrentBGM)
    {
        mCurrentBGM->FadeOut(FadeDuration, 0.0f);  // フェードアウト
    }
}