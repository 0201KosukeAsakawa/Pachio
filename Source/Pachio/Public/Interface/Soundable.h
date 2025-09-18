// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Soundable.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class USoundable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PACHIO_API ISoundable
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	//音量設定
	virtual void SetBGMVolume(float vol);
	virtual void SetSEVolume(float vol);
	//サウンドの再生(フェードイン))
	virtual void PlaySoundWithFadeIn(FName DataID, FName SoundID, float Volume, float FadeDuration);
	//BGMの停止(完全停止)
	virtual void StopBGM();
	//BGMの停止(フェードアウト)
	virtual void StopBGMWithFadeOut(float FadeDuration);
	//BGMの音量取得
	virtual float GetBGMVolume()const;
	//SEの音量取得
	virtual float GetSEVolume()const;
	// サウンドを再生
	virtual bool PlaySound(FName DataID, FName SoundID,const bool SetVolume = false, float Volume = 0, bool IsSpecifyLocation = false, FVector place = FVector(0, 0, 0));
};
