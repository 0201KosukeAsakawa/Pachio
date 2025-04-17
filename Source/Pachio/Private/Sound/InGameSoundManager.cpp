#include "Sound/InGameSoundManager.h"
#include "EngineUtils.h"
#include "Sound/SoundManager.h" // 念のため

TWeakObjectPtr<AInGameSoundManager> AInGameSoundManager::Instance = nullptr;

AInGameSoundManager::AInGameSoundManager()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AInGameSoundManager::BeginPlay()
{
	Super::BeginPlay();

	// シングルトン登録
	Instance = this;

	// SoundManager取得（型をテンプレートではなく明示指定）
	if (!SoundManager)
		SoundManager = Cast<USoundManager>(GetComponentByClass(USoundManager::StaticClass()));

	if (SoundManager)
	{
		SoundManager->Init();
		SoundManager->PlaySound("BGM", "Default", SoundManager->GetBGMVolume());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("SoundManager component not found on AInGameSoundManager."));
	}
}

void AInGameSoundManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

AInGameSoundManager* AInGameSoundManager::Get(UObject* WorldContext)
{
	if (Instance.IsValid())
	{
		return Instance.Get();
	}

	UWorld* World = WorldContext ? WorldContext->GetWorld() : nullptr;
	if (!World) return nullptr;

	for (TActorIterator<AInGameSoundManager> It(World); It; ++It)
	{
		Instance = *It;
		return *It;
	}

	return nullptr;
}

void AInGameSoundManager::PlaySound(FName WaveName, FName SoundName)
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