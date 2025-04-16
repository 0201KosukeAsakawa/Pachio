#include "Interface/Soundable.h"

// Add default functionality here for any ISoundable functions that are not pure virtual.

void ISoundable::SetSoundVolume(float BGMvol, float SEVol)
{
}

void ISoundable::PlaySoundWithFadeIn(FName DataID, FName SoundID, float Volume, float FadeDuration)
{
}

void ISoundable::StopBGM(bool b)
{
}

void ISoundable::StopBGMWithFadeOut(float FadeDuration)
{
}

float ISoundable::GetBGMVolume() const
{
	return 0.0f;
}

float ISoundable::GetSEVolume() const
{
	return 0.0f;
}

bool ISoundable::PlaySound(FName DataID, FName SoundID)
{
	return false;
}

bool ISoundable::PlaySound(FName DataID, FName SoundID, float Volume, bool IsSpecifyLocation, FVector place)
{
	return false;
}
