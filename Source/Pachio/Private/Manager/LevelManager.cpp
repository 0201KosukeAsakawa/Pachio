#include "Manager/LevelManager.h"
#include "Manager/ObjectManager.h"
#include "EngineUtils.h"
#include "Engine/DataTable.h"
#include "DataContainer/BlockDataContainer.h"
#include "DataContainer/AttackDataContainer.h"
#include "Sound/SoundManager.h"

// シングルトン用の静的インスタンス
TWeakObjectPtr<ALevelManager> ALevelManager::Instance = nullptr;

// コンストラクタ：Tickの有効化
ALevelManager::ALevelManager()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ALevelManager::BeginPlay()
{
	Super::BeginPlay();

	// シングルトンインスタンスとして登録
	Instance = this;

	// BlockContainerの初期化
	if (!IsValid(BlockContainer))
	{
		BlockContainer = NewObject<UBlockDataContainer>(this, ContainerClass);
	}

	// アイテムデータコンテナの初期化
	ItemContainer = NewObject<UItemDataContainer>(this, ItemContainerClass);

	AttackContainer = NewObject<UAttackDataContainer>(this, AttackContainerClass);

	// ObjectManagerの初期化（テンプレートから複製）
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

	// サウンドマネージャの初期化とBGM再生
	if (!IsValid(SoundManager))
	{
		SoundManager = NewObject<USoundManager>(this, SoundManagerClass);
		SoundManager->Init();
		SoundManager->PlaySound("BGM", "Default", SoundManager->GetBGMVolume());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("SoundManager component not found on ALevelManager."));
	}

	// ステージ生成とブロック生成を実行
	GenerateStage();
	GenerateBlock();
}

void ALevelManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// シングルトンインスタンスの取得関数
ALevelManager* ALevelManager::GetInstance(UObject* WorldContext)
{
	if (Instance.IsValid())
	{
		return Instance.Get();
	}

	UWorld* World = WorldContext ? WorldContext->GetWorld() : nullptr;
	if (!World)
		return nullptr;

	// ワールド内から検索
	for (TActorIterator<ALevelManager> It(World); It; ++It)
	{
		Instance = *It;
		return *It;
	}

	return nullptr;
}

// 任意のサウンドを再生
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

// データテーブルからステージオブジェクトを生成
void ALevelManager::GenerateStage()
{
	if (StageData == nullptr)
	{
		// 非同期ロードされていない場合は同期的に読み込む
		StageData.LoadSynchronous();
	}

	if (StageData)
	{
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
				ObjectManager->GenerateObject(data->ObjectName, data->MaterialName, location, scale, rotate);
		}
	}
}

// データテーブルからブロックを生成
void ALevelManager::GenerateBlock()
{
	if (BlockData == nullptr)
	{
		BlockData.LoadSynchronous();
	}

	if (BlockData)
	{
		TArray<FName> RowNames = BlockData->GetRowNames();
		for (auto RowName : RowNames)
		{
			const FInstansBlockData* data = BlockData->FindRow<FInstansBlockData>(RowName, FString());
			if (!data)
			{
				continue;
			}

			FVector location = FVector(data->Location_X, data->Location_Y, data->Location_Z);
			FVector scale = FVector(data->Scale_X, data->Scale_Y, data->Scale_Z);
			FRotator rotate = FRotator(data->Rotate_X, data->Rotate_Y, data->Rotate_Z);

			if (BlockContainer)
				BlockContainer->GenerateBlock(data->StateID, data->DropItem, data->MaterialID, location, scale, rotate);
		}
	}
}