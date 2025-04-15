// Copyright Epic Games, Inc. All Rights Reserved.

#include "PachioGameMode.h"
#include "PachioCharacter.h"
#include "UObject/ConstructorHelpers.h"

APachioGameMode::APachioGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
