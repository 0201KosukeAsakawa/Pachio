#include "Manager/LevelManager.h"
#include "Manager/ObjectManager.h"
#include "EngineUtils.h"
#include "Engine/DataTable.h"
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
	if (!IsValid(BlockContainer))
	{
		BlockContainer = NewObject<UBlockDataContainer>(this, ContainerClass);
	}
	ItemContainer = NewObject<UItemDataContainer>(this, ItemContainerClass);
	if (!IsValid(ObjectManager))
	{
		ObjectManager = NewObject<UObjectManager>(this, ObjectManagerClass);

		if (ObjectManager && ObjectManagerClass)
		{
			UObjectManager* DefaultObj = Cast<UObjectManager>(ObjectManagerClass->GetDefaultObject());
			if (DefaultObj)
			{
				ObjectManager->DuplicateContentsFrom(DefaultObj);
			}
		}
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
	GenerateStage();
	GenerateBlock();
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

void ALevelManager::GenerateStage()
{
	if (StageData == nullptr)
	{
		// 参照できない場合は同期読み.
		StageData.LoadSynchronous();
	}

	if (StageData)
	{
		// データテーブルから全データを取得する.
		TArray<FName> RowNames = StageData->GetRowNames();
		for (auto RowName : RowNames)
		{
			const FStageData* data = StageData->FindRow<FStageData>(RowName, FString());
			if (!data)
			{
				continue;
			}

			UE_LOG(LogTemp, Log, TEXT("[%s]:[%f][%f][%f]"), *RowName.ToString(), data->Scale_X, data->Scale_Y, data->Scale_Z);
			FVector location = FVector(data->Location_X, data->Location_Y, data->Location_Z);
			FVector scale = FVector(data->Scale_X, data->Scale_Y, data->Scale_Z);
			FRotator rotate = FRotator(data->Rotate_X, data->Rotate_Y, data->Rotate_Z);
			if (ObjectManager)
				ObjectManager->GenerateObject(data->ObjectName,data->MaterialName ,location,scale, rotate);
		}
	}
}

void ALevelManager::GenerateBlock()
{
	if (BlockData == nullptr)
	{
		// 参照できない場合は同期読み.
		BlockData.LoadSynchronous();
	}

	if (BlockData)
	{
		// データテーブルから全データを取得する.
		TArray<FName> RowNames = BlockData->GetRowNames();
		for (auto RowName : RowNames)
		{
			const FInstansBlockData* data = BlockData->FindRow<FInstansBlockData>(RowName, FString());
			if (!data)
			{
				continue;
			}
			FVector location = FVector(data->Location_X, data->Location_Y, data->Location_Z);
			FVector scalse = FVector(data->Scale_X, data->Scale_Y,data->Scale_Z);
			FRotator rotate = FRotator(data->Rotate_X, data->Rotate_Y, data->Rotate_Z);
			if (BlockContainer)
				BlockContainer->GenerateBlock(data->StateID,data->DropItem, data->MaterialID, location, scalse,rotate);
		}
	}
}
