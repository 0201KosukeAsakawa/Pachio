#include "Sound/SoundManager.h"
#include "Kismet/GameplayStatics.h"
#include "Logic/ColorManager/ColorTargetRegistry.h"
#include "Components/AudioComponent.h"
#include "ColorUtilityLibrary.h"
#include "Manager/LevelManager.h"
#include "Manager/ColorManager.h"
#include "Manager/SaveManager.h"

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

// =======================
// コンストラクタ
// =======================

USoundManager::USoundManager()
    : BGMVolume(1)
    , SEVolume(1)
    , mCurrentBGM(nullptr)
    , MusicBPM(100.f)
    , BeatInterval(5.f)
    , StartTime(0.f)
    , LastPredictedBeat(-1.f)
    , LastConfirmedBeatTime(0.f)
{
}

// =======================
// 初期化処理
// =======================

void USoundManager::Init()
{
    // 保存データから音量を読み込み
    LoadOrCreateVolumeSave();

    // 登録されているサウンドデータを AudioComponent に初期化
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

            // 既に登録済みならスキップ
            if (soundData.AudioComponentMap.Contains(waveTag))
                continue;

            // 2D サウンドコンポーネントを生成
            UAudioComponent* AudioComponent = UGameplayStatics::CreateSound2D(this, sound);
            if (AudioComponent == nullptr)
                continue;

            AudioComponent->bAutoDestroy = false;

            // BGM の場合は Envelope イベントを監視
            if (dataTag == "BGM")
            {
                AudioComponent->OnAudioSingleEnvelopeValue.AddDynamic(this, &USoundManager::OnEnvelopeValue);
            }

            soundData.AudioComponentMap.Add(waveTag, AudioComponent);
        }
    }

    // 色変化に応じた BPM 変更イベントをバインド
    ALevelManager::GetInstance(GetWorld())->GetColorManager()->GetColorTargetRegistry()->OnColorApplied.AddDynamic(this, &USoundManager::SetTmp);
}

// =======================
// 音量の保存・読み込み
// =======================

// 保存されている音量データをロード、なければ作成
void USoundManager::LoadOrCreateVolumeSave()
{
    FVolumeSaveData LoadedData = USaveManager::LoadVolumeFromJson();
    BGMVolume = LoadedData.BGMVolume;
    SEVolume = LoadedData.SEVolume;
}

// 音量を保存
void USoundManager::SetVolume(float NewBGM, float NewSE)
{
    BGMVolume = NewBGM;
    SEVolume = NewSE;
    USaveManager::SetVolume(BGMVolume, SEVolume);
}

// =======================
// 色変化による BPM 設定
// =======================

void USoundManager::SetTmp(FLinearColor NewColor)
{
    ALevelManager* level = ALevelManager::GetInstance(GetWorld());
    if (!level) return;

    UColorManager* colorManager = level->GetColorManager();
    if (!colorManager) return;

    // 色から最も近いエフェクトを判定
    EColorCategory colorCategory = UColorUtilityLibrary::GetNearestPrimaryColor(NewColor);

    // 色に応じて BPM を設定
    switch (colorCategory)
    {
    case EColorCategory::Red:   MusicBPM = 160.f; break;
    case EColorCategory::Blue:  MusicBPM = 90.f; break;
    case EColorCategory::Green: MusicBPM = 120.f; break;
    default:                 MusicBPM = 120.f; break;
    }

    // 1拍の秒数を計算
    BeatInterval = 60.f / MusicBPM;

    // ビートタイマーをリセット
    StartTime = GetWorld()->GetTimeSeconds();
    LastPredictedBeat = -1;
}

// =======================
// 音量制御
// =======================

// BGM 音量設定
void USoundManager::SetBGMVolume(float vol)
{
    const float previousBGMVolume = BGMVolume;

    // 音量を 0〜4 の範囲に制限
    BGMVolume = FMath::Clamp(vol, 0, 4.0f);

    if (BGM)
    {
        BGM->SetVolume(BGMVolume);
    }

    // 音量が変化した場合のみ処理
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

// SE 音量設定
void USoundManager::SetSEVolume(float vol)
{
    SEVolume = FMath::Clamp(vol, 0.0f, 4.0f);
}

// =======================
// サウンド再生処理
// =======================

bool USoundManager::PlaySound(FName DataID, FName SoundID, bool SetVolume, float Volume, bool IsSpecifyLocation, FVector place)
{
    float volume = 0;
    if (!SetVolume)
    {
        if (DataID == "BGM")      volume = BGMVolume;
        else if (DataID == "SE")  volume = SEVolume;
    }

    // BGM 再生の場合
    if (DataID == "BGM")
    {
        PlayBGM();
        return true;
    }

    // サウンドデータが存在するかチェック
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

    // 音量設定
    const float volumeToPlay = FMath::Clamp(volume, 0.0f, 1.0f);
    AudioComponent->SetVolumeMultiplier(volumeToPlay);

    // 位置指定がある場合のみワールド位置を設定
    if (IsSpecifyLocation)
    {
        AudioComponent->SetWorldLocation(place);
    }

    // 距離減衰設定（必要なら適用）
    FSoundAttenuationSettings AttenuationSettings;
    AttenuationSettings.bAttenuate = true;
    AttenuationSettings.FalloffDistance = 2000.0f;

    // サウンド再生
    AudioComponent->Play();
    return true;
}

// =======================
// BGM 制御
// =======================

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
    if (!AudioComponent) return;

    AudioComponent->SetVolumeMultiplier(FMath::Clamp(Volume, 0.0f, 1.0f));
    AudioComponent->FadeIn(FadeDuration);
    AudioComponent->Play();
}

void USoundManager::StopBGMWithFadeOut(float FadeDuration)
{
    // TODO: BGM フェードアウト処理を実装
}

// =======================
// サウンド解析
// =======================

// 音声のエンベロープ値を取得（未使用）
void USoundManager::OnEnvelopeValue(const USoundWave* SoundWave, const float EnvelopeValue)
{
}

// =======================
// BGM 再生 (FMOD 使用)
// =======================

bool USoundManager::PlayBGM()
{
    if (!BGMEventAsset) return false;

    BeatInterval = 60.0f / MusicBPM;

    if (!BGM)
    {
        BGM = NewObject<UFMODAudioComponent>(this);
        BGM->RegisterComponent();
    }

    BGM->SetEvent(BGMEventAsset);
    BGM->Play();

    EventInstance = BGM->StudioInstance;
    if (EventInstance)
    {
      
    }

    StartTime = GetWorld()->GetTimeSeconds();
    LastPredictedBeat = -1;

    return true;
}
