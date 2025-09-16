#include "Sound/SoundManager.h"
#include "Kismet/GameplayStatics.h"
#include "Logic/ColorManager/ColorTargetRegistry.h"
#include "Components/AudioComponent.h"
#include "Manager/LevelManager.h"
#include "Manager/ColorManager.h"

// FMODの低レベルAPIのヘッダーをインクルードします。
// F_CALLBACK マクロが正しく定義されるように、FMOD_API_TRUE または FMOD_STUDIO_API_TRUE を定義します。
// これは、コンパイラがFMODのAPI関数の正しい呼び出し規約（例: __stdcall）を使用するように指示します。
#define FMOD_API_TRUE
#define FMOD_STUDIO_API_TRUE // FMOD Studio APIを使用している場合、これも定義します。

// FMODのコアAPIとStudio APIのヘッダー
#include "fmod_studio.h"
#include "fmod.h"
#include "fmod_common.h"       // F_CALLBACK マクロの定義が含まれることが多い
#include "fmod_studio.hpp"     // FMOD Studio APIのC++ラッパー
#include "fmod_studio_common.h" // FMOD Studioの共通定義（コールバック関連も含む）

// FMODAudioComponentの定義が必要なため、インクルードします。
// UFMODAudioComponent::EventInstance メンバーにアクセスするために必要です。
#include "FMODAudioComponent.h"



static FMOD_RESULT OnTimelineMarker(FMOD_STUDIO_EVENT_CALLBACK_TYPE type, FMOD_STUDIO_EVENTINSTANCE* eventInstance, void* parameters)
{
    if (type == FMOD_STUDIO_EVENT_CALLBACK_TIMELINE_MARKER)
    {
        auto* Marker = static_cast<FMOD_STUDIO_TIMELINE_MARKER_PROPERTIES*>(parameters);
        FString MarkerName = UTF8_TO_TCHAR(Marker->name);

        if (MarkerName == "Beat")
        {
            void* RawUserData = nullptr;
            ((FMOD::Studio::EventInstance*)eventInstance)->getUserData(&RawUserData);
            USoundManager* Manager = static_cast<USoundManager*>(RawUserData);
            if (Manager)
            {
                Manager->OnMarkerBeat(Marker->position);
            }
        }
    }
    return FMOD_OK;
}

USoundManager::USoundManager()
    : BGMVolume(1)
    , SEVolume(1)
    , mCurrentBGM(nullptr)
{
}

void USoundManager::Init()
{
    for (auto& soundMap : SoundDataMap)
    {
        const FName dataTag = soundMap.Key;
        FSoundData& soundData = soundMap.Value;
        soundData.AudioComponentMap.Reset();
        for (const auto& soundAssetPair : soundData.SoundAssetMap)
        {
            const FName waveTag = soundAssetPair.Key;
            USoundBase* sound = soundAssetPair.Value;
            if (waveTag.IsNone() || !sound)
                continue;

            if (soundData.AudioComponentMap.Contains(waveTag))
                continue;

            UAudioComponent* AudioComponent = UGameplayStatics::CreateSound2D(this, sound);
            if (AudioComponent == nullptr)
                continue;

            AudioComponent->bAutoDestroy = false;

            if (dataTag == "BGM")
            {
                AudioComponent->OnAudioSingleEnvelopeValue.AddDynamic(this, &USoundManager::OnEnvelopeValue);
            }

            soundData.AudioComponentMap.Add(waveTag, AudioComponent);

        }
    }
    ALevelManager::GetInstance(GetWorld())->GetColorManager()->GetColorTargetRegistry()->OnColorApplied.AddDynamic(this, &USoundManager::SetTmp);
    InitTestSound();
}

void USoundManager::Tick(float DeltaTime)
{
   /* float Now = GetWorld()->GetTimeSeconds();
    float Elapsed = Now - StartTime;

    int32 CurrentBeat = FMath::FloorToInt(Elapsed / BeatInterval);
    if (CurrentBeat > LastPredictedBeat)
    {
        LastPredictedBeat = CurrentBeat;
        OnBeatDetected.Broadcast();
    }*/
}

void USoundManager::SetTmp(EColorTargetType Mode, FLinearColor NewColor)
{
    ALevelManager* level = ALevelManager::GetInstance(GetWorld());
    if (level == nullptr)
        return;

    UColorManager* colorManager = level->GetColorManager();
    if (colorManager == nullptr)
        return;

    FEffectMatchResult Match = ALevelManager::GetInstance(GetWorld())
        ->GetColorManager()
        ->GetClosestEffectByHue(NewColor);

    switch (Match.ClosestEffect)
    {

        case EBuffEffect::Red:
            MusicBPM = 160.f;
            break;
        case EBuffEffect::Blue:
            MusicBPM = 90.f;
            break;
        case EBuffEffect::Green:
            MusicBPM = 120.f;
            break;
        default:
            MusicBPM = 120.f;
            break;
    }
        BeatInterval = 60.f / MusicBPM;  // 拍の秒数を計算

        StartTime = GetWorld()->GetTimeSeconds();  // ビート判定の基準時間をリセット
        LastPredictedBeat = -1;  // 拍判定リセット
    
    colorManager->GetColorTargetRegistry();
    
}

void USoundManager::SetBGMVolume(float vol)
{
    const float previousBGMVolume = BGMVolume;

    // 音量を0〜1の範囲に制限
    BGMVolume = FMath::Clamp(vol, 0.0f, 1.0f);

    // 音量をFMODに反映
    if (BGM)
    {
        BGM->SetVolume(BGMVolume); // ✅ ここが重要！
    }

    // 音量が変更された場合のみ処理
    if (BGM && previousBGMVolume != BGMVolume)
    {
        if (BGMVolume > 0.0f && !BGM->IsPlaying())
        {
            BGM->Play();
            BGM->SetVolume(BGMVolume);
        }
        else if (BGMVolume == 0.0f)
        {
            BGM->Stop();
        }
    }
}

bool USoundManager::PlaySound(FName DataID, FName SoundID, float Volume, bool IsSpecifyLocation, FVector place)
{
    if (DataID == "BGM")
    {
        PlayBGM();
       return true;
    }

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

    UAudioComponent* AudioComponent = Cast<UAudioComponent>(SoundDataMap[DataID].AudioComponentMap[SoundID]);
    if (!AudioComponent)
    {
        UE_LOG(LogTemp, Error, TEXT("AudioComponent is null for SoundID: %s in DataID: %s"), *SoundID.ToString(), *DataID.ToString());
        Init();
        return false;
    }

    // 音量を設定 (0.0が無音、1.0が最大音量)
    const float volumeToPlay = FMath::Clamp(Volume, 0.0f, 1.0f);
    AudioComponent->SetVolumeMultiplier(volumeToPlay);

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

void USoundManager::StopBGM()
{
    if (FMODAudioComponent)
    {
        FMODAudioComponent->Stop();
    }
}

void USoundManager::PlaySoundWithFadeIn(FName DataID, FName SoundID, float Volume, float FadeDuration)
{
    if (!SoundDataMap.Contains(DataID) || !SoundDataMap[DataID].AudioComponentMap.Contains(SoundID))
        return;

    UAudioComponent* AudioComponent = Cast<UAudioComponent>(SoundDataMap[DataID].AudioComponentMap[SoundID]);
    if (!AudioComponent)
        return;

    AudioComponent->SetVolumeMultiplier(FMath::Clamp(Volume, 0.0f, 1.0f));
    AudioComponent->FadeIn(FadeDuration);  // フェードインを追加
    AudioComponent->Play();
}

void USoundManager::StopBGMWithFadeOut(float FadeDuration)
{
    //if (mCurrentBGM)
    //{
    //    mCurrentBGM->FadeOut(FadeDuration, 0.0f);  // フェードアウト
    //}
}

void USoundManager::OnEnvelopeValue(const USoundWave* SoundWave, const float EnvelopeValue)
{
}

// PlayBGM() を修正
bool USoundManager::PlayBGM()
{
    if (!BGMEventAsset) return false;

    BeatInterval = 60.0f / MusicBPM;

    if (!BGM) // ← 統一
    {
        BGM = NewObject<UFMODAudioComponent>(this);
        BGM->RegisterComponent();
    }

    BGM->SetEvent(BGMEventAsset);
    BGM->Play();

    EventInstance = BGM->StudioInstance;
    if (EventInstance)
    {
        EventInstance->setUserData(this);
        EventInstance->setCallback(OnTimelineMarker, FMOD_STUDIO_EVENT_CALLBACK_TIMELINE_MARKER);
    }

    StartTime = GetWorld()->GetTimeSeconds();
    LastPredictedBeat = -1;

    return true;
}

void USoundManager::OnBeatTimerElapsed()
{
    // ここで通知
    //OnBeatDetected.Broadcast();
}

void USoundManager::InitTestSound()
{
    if (!BGM)
    {
        BGM = NewObject<UFMODAudioComponent>(this);
        BGM->RegisterComponent();

        if (BGMEventAsset)
        {
            BGM->SetEvent(BGMEventAsset);  // ← これが MyFMODEventAsset 相当
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("TestEventAsset is null!"));
        }
    }
}

void USoundManager::OnMarkerBeat(int64 MarkerPositionMs)
{
    LastConfirmedBeatTime = MarkerPositionMs / 1000.0f;
    OnBeatDetected.Broadcast();

}
