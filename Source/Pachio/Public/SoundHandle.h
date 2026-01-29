// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SoundHandle.generated.h"


class ISoundManagerProvider;
enum class ESoundKinds : uint8;
/**
 *
 */
UCLASS()
class PACHIO_API USoundHandle : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    // ==========================
    // ==== ���ʐ��� ====
    // ==========================

    /**
     * @brief BGM/SE���ʂ�ꊇ�ݒ�
     * @param WorldContext ���[���h�R���e�L�X�g
     * @param NewBGM BGM���ʁi0-1�j
     * @param NewSE SE���ʁi0-1�j
     */
    UFUNCTION(BlueprintCallable, Category = "Sound|Volume", meta = (WorldContext = "WorldContext"))
    static void SetVolume(UObject* WorldContext, float NewBGM, float NewSE);

    /**
     * @brief BGM���ʐݒ�
     * @param WorldContext ���[���h�R���e�L�X�g
     * @param Volume ���ʁi0-1�j
     */
    UFUNCTION(BlueprintCallable, Category = "Sound|Volume", meta = (WorldContext = "WorldContext"))
    static void SetBGMVolume(UObject* WorldContext, float Volume);

    /**
     * @brief SE���ʐݒ�
     * @param WorldContext ���[���h�R���e�L�X�g
     * @param Volume ���ʁi0-1�j
     */
    UFUNCTION(BlueprintCallable, Category = "Sound|Volume", meta = (WorldContext = "WorldContext"))
    static void SetSEVolume(UObject* WorldContext, float Volume);

    /**
     * @brief ���݂�BGM���ʂ�擾
     * @param WorldContext ���[���h�R���e�L�X�g
     * @return float BGM���ʁi0-1�j
     */
    UFUNCTION(BlueprintPure, Category = "Sound|Volume", meta = (WorldContext = "WorldContext"))
    static float GetBGMVolume(UObject* WorldContext);

    /**
     * @brief ���݂�SE���ʂ�擾
     * @param WorldContext ���[���h�R���e�L�X�g
     * @return float SE���ʁi0-1�j
     */
    UFUNCTION(BlueprintPure, Category = "Sound|Volume", meta = (WorldContext = "WorldContext"))
    static float GetSEVolume(UObject* WorldContext);

    // ==========================
    // ==== BGM���� ====
    // ==========================

    /**
     * @brief BGM���~
     * @param WorldContext ���[���h�R���e�L�X�g
     */
    UFUNCTION(BlueprintCallable, Category = "Sound|BGM", meta = (WorldContext = "WorldContext"))
    static void StopBGM(UObject* WorldContext);


    /**
     * @brief BGM���~
     * @param WorldContext ���[���h�R���e�L�X�g
     * @param SEName       ��~������SE��
     */
    UFUNCTION(BlueprintCallable, Category = "Sound|SE", meta = (WorldContext = "WorldContext"))
    static void StopSE(UObject* WorldContext, FName SEName);
    // ==========================
    // ==== �T�E���h�Đ� ====
    // ==========================

    /**
     * @brief �T�E���h��Đ�
     * @param WorldContext ���[���h�R���e�L�X�g
     * @param DataID �T�E���h�f�[�^ID�i"BGM", "SE"�Ȃǁj
     * @param SoundID �Đ�����T�E���h��ID
     * @param SetVolume �J�X�^�����ʂ�g�p���邩
     * @param Volume �J�X�^�����ʁi0-1�j
     * @param IsSpecifyLocation �ʒu�w��Đ����邩
     * @param Location �Đ��ʒu
     * @return bool �Đ��������ۂ�
     */
    UFUNCTION(BlueprintCallable, Category = "Sound|Playback", meta = (WorldContext = "WorldContext", AdvancedDisplay = "SetVolume,Volume,IsSpecifyLocation,Location"))
    static bool PlaySound(
        UObject* WorldContext,
        ESoundKinds DataID,
        FName SoundID,
        bool isLoop = false,
        bool SetVolume = false,
        float Volume = 1.0f,
        bool IsSpecifyLocation = false,
        FVector Location = FVector::ZeroVector
    );
    // ==========================
    // ==== �֗��֐� ====
    // ==========================

    /**
     * @brief SE��ȈՍĐ��i�f�t�H���g���ʂ�2D�Đ��j
     * @param WorldContext ���[���h�R���e�L�X�g
     * @param SoundID �Đ�����SE��ID
     * @param isLoop �J��Ԃ��Đ����邩
     * @return bool �Đ��������ۂ�
     */
    UFUNCTION(BlueprintCallable, Category = "Sound|Playback", meta = (WorldContext = "WorldContext"))
    static bool PlaySE(UObject* WorldContext, FName SoundID, const bool isLoop = false);

    /**
     * @brief SE��ʒu�w��ōĐ�
     * @param WorldContext ���[���h�R���e�L�X�g
     * @param SoundID �Đ�����SE��ID
     * @param Location �Đ��ʒu
     * @param isLoop �J��Ԃ��Đ������邩
     * @return bool �Đ��������ۂ�
     */
    UFUNCTION(BlueprintCallable, Category = "Sound|Playback", meta = (WorldContext = "WorldContext"))
    static bool PlaySEAtLocation(UObject* WorldContext, FName SoundID, FVector Location, bool isLoop);

private:
    /**
     * @brief SoundManager�C���X�^���X��擾
     * @param WorldContext ���[���h�R���e�L�X�g
     * @return ISoundableProvider �C���^�[�t�F�[�X
     */
    static TScriptInterface<ISoundManagerProvider> GetSoundManager(UObject* WorldContext);
};