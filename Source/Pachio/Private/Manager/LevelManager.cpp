#include "Manager/LevelManager.h"
#include "EngineUtils.h"
#include "DataContainer/BlockDataContainer.h"
#include "Sound/SoundManager.h"

TWeakObjectPtr<ALevelManager> ALevelManager::Instance = nullptr;

ALevelManager::ALevelManager()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ALevelManager::BeginPlay()
{
	Super::BeginPlay();

	// シングルトン登録
	Instance = this;

	//
	if (!IsValid(Container))
	{
		Container = NewObject<UBlockDataContainer>(this, ContainerClass);
	}

	if (!IsValid(SoundManager))
	{
		SoundManager = NewObject<USoundManager>(this,SoundManagerClass);
		SoundManager->Init();
		SoundManager->PlaySound("BGM", "Default", SoundManager->GetBGMVolume());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("SoundManager component not found on ALevelManager."));
	}
}

void ALevelManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

ALevelManager* ALevelManager::GetComponent(UObject* WorldContext)
{
	if (Instance.IsValid())
	{
		return Instance.Get();
	}

	UWorld* World = WorldContext ? WorldContext->GetWorld() : nullptr;
	if (!World)
		return nullptr;

	for (TActorIterator<ALevelManager> It(World); It; ++It)
	{
		Instance = *It;
		return *It;
	}

	return nullptr;
}

void ALevelManager::PlaySound(FName WaveName, FName SoundName)
{
	if (SoundManager)
	{
		SoundManager->PlaySound(WaveName, SoundName);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("SoundManager is null when trying to play sound."));
	}
}