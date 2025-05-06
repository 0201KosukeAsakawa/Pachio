#include "Manager/LevelManager.h"
#include "Manager/ObjectManager.h"
#include "Manager/ScoreManager.h"
#include "UI/UIManager.h"
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
	PrimaryActorTick.bCanEverTick = true; // Tickを有効にする
}

void ALevelManager::BeginPlay()
{
	Super::BeginPlay();
	Instance = this;

	// コンポーネント初期化
	InitializeComponents();
}

void ALevelManager::InitializeComponents()
{
	if (!IsValid(BlockContainer))
		BlockContainer = NewObject<UBlockDataContainer>(this, BlockContainerClass);

	ItemContainer = NewObject<UItemDataContainer>(this, ItemContainerClass);
	AttackContainer = NewObject<UAttackDataContainer>(this, AttackContainerClass);
	ScoreManager = NewObject<UScoreManager>(this, ScoreManagerClass);
	UIManager = NewObject<UUIManager>(this, UIManagerClass);

	if (UIManager)
	{
		UIManager->Init();
		UIManager->ShowWidget("UI", "Score");
	}

	if (!IsValid(ObjectManager))
	{
		ObjectManager = NewObject<UObjectManager>(this, ObjectManagerClass);
		if (ObjectManager && ObjectManagerClass)
		{
			if (auto* DefaultObj = Cast<UObjectManager>(ObjectManagerClass->GetDefaultObject()))
			{
				ObjectManager->DuplicateContentsFrom(DefaultObj);
			}
		}
	}

	if (!IsValid(SoundManager))
	{
		SoundManager = NewObject<USoundManager>(this, SoundManagerClass);
		if (SoundManager)
		{
			SoundManager->Init();
			SoundManager->PlaySound("BGM", "Default", SoundManager->GetBGMVolume());
		}
	}

	// 1秒ごとに CountUp 関数を呼ぶ
	GetWorld()->GetTimerManager().SetTimer(CountTimerHandle, this, &ALevelManager::CountDown, 1.0f, true);

	GenerateStage();
	GenerateBlock();
}

void ALevelManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// GetInstance関数で、インスタンスが未設定の場合は初期化処理を強制する
ALevelManager* ALevelManager::GetInstance(UObject* WorldContext)
{
	if (!Instance.IsValid())
	{
		// WorldContextからレベルマネージャーを検索
		UWorld* World = WorldContext ? WorldContext->GetWorld() : nullptr;
		if (!World)
			return nullptr;

		// インスタンスがまだ設定されていない場合
		for (TActorIterator<ALevelManager> It(World); It; ++It)
		{
			Instance = *It;
			Instance->InitializeComponents();  // 必要な初期化処理を行う
			return *It;
		}

		// レベルマネージャーが見つからなければ、新しく生成
		ALevelManager* NewInstance = World->SpawnActor<ALevelManager>();
		Instance = NewInstance;
		Instance->InitializeComponents();  // 必要な初期化処理を行う
	}

	return Instance.Get();
}

void ALevelManager::PlaySound(FName WaveName, FName SoundName)
{
	if (!SoundManager)
	{
		UE_LOG(LogTemp, Warning, TEXT("SoundManager is null when trying to play sound."));
		return;
	}

	SoundManager->PlaySound(WaveName, SoundName);
}

void ALevelManager::GenerateStage()
{
	if (!StageData.IsValid())
		StageData.LoadSynchronous();

	if (!StageData)
		return;

	for (FName RowName : StageData->GetRowNames())
	{
		const FStageData* data = StageData->FindRow<FStageData>(RowName, FString());
		if (!data)
			continue;

		FVector location(data->Location_X, data->Location_Y, data->Location_Z);
		FVector scale(data->Scale_X, data->Scale_Y, data->Scale_Z);
		FRotator rotate(data->Rotate_X, data->Rotate_Y, data->Rotate_Z);

		if (ObjectManager)
			ObjectManager->GenerateObject(data->ObjectName, data->MaterialName, location, scale, rotate);
	}
}

void ALevelManager::GenerateBlock()
{
	if (!BlockData.IsValid())
		BlockData.LoadSynchronous();

	if (!BlockData)
		return;

	for (FName RowName : BlockData->GetRowNames())
	{
		const FInstansBlockData* data = BlockData->FindRow<FInstansBlockData>(RowName, FString());
		if (!data)
			continue;

		FVector location(data->Location_X, data->Location_Y, data->Location_Z);
		FVector scale(data->Scale_X, data->Scale_Y, data->Scale_Z);
		FRotator rotate(data->Rotate_X, data->Rotate_Y, data->Rotate_Z);

		if (BlockContainer)
			BlockContainer->GenerateBlock(data->StateID, data->DropItem, data->MaterialID, location, scale, rotate);
	}
}

void ALevelManager::CountDown()
{
	InGameTimer--;
}