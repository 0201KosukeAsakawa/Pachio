#include "Manager/LevelManager.h"
#include "Manager/ObjectManager.h"
#include "Manager/ScoreManager.h"
#include "Manager/ColorManager.h"
#include "Manager/SaveManager.h"
#include "Manager/WeatherEffectManager.h"
#include "Kismet/GameplayStatics.h" 
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
	if (bInitialize)
		return;
	if (ScoreManagerClass)
		ScoreManager = NewObject<UScoreManager>(this, ScoreManagerClass);
	if (ScoreManager)
		ScoreManager->Init();
	if (SoundManagerClass)
		SoundManager = NewObject<USoundManager>(this, SoundManagerClass);
	if (SoundManager)
	{
		SoundManager->Init();
		SoundManager->PlaySound("BGM", "Default", SoundManager->GetBGMVolume());
	}
	if (UIManagerClass)
		UIManager = NewObject<UUIManager>(this, UIManagerClass);
	if (ColorManagerClass)
	{
		ColorManager = NewObject<UColorManager>(this, ColorManagerClass);
		ColorManager->Init();
	}

	if (UIManager)
	{
		UIManager->Init(this);
	}

	bInitialize = true;
}

void ALevelManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (SoundManager)
		SoundManager->Tick(DeltaTime);
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

TScriptInterface<ISoundable> ALevelManager::GetSoundManager() const
{
	return TScriptInterface<ISoundable>(SoundManager);
}


void ALevelManager::HandlePlayerGoalReached()
{
	if (!ScoreManager || !UIManager) return;

	float ClearTime = ScoreManager->GetTime();
	EStageRank Rank = ScoreManager->EvaluateClearRank(GetWorld());

	UUserWidget* ResultWidget = UIManager->ShowResultWidget(ClearTime, Rank);
	SoundManager->StopBGM();
	SoundManager->PlaySound("SE", "Fanfare");
	PauseGameAndShowUI(ResultWidget);
}

void ALevelManager::PauseGameAndShowUI(UUserWidget* FocusWidget)
{
	UGameplayStatics::SetGamePaused(GetWorld(), true);
	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (!PC)
		return;

	PC->bShowMouseCursor = true;
	FInputModeUIOnly InputMode;

	if (FocusWidget)
	{
		InputMode.SetWidgetToFocus(FocusWidget->TakeWidget());
	}
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

	PC->SetInputMode(InputMode);

}
