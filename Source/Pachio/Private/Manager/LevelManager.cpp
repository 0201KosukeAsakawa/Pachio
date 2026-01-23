#include "Manager/LevelManager.h"
#include "Manager/ScoreManager.h"
#include "Manager/ColorManager.h"
#include "Manager/SaveManager.h"
#include "Manager/WeatherEffectManager.h"
#include "Kismet/GameplayStatics.h" 
#include "UI/UIManager.h"
#include "EngineUtils.h"
#include "Engine/DataTable.h"
#include "Sound/SoundManager.h"

// =======================
// 静的シングルトンインスタンス
// =======================

// シングルトン用の静的インスタンス
TWeakObjectPtr<ALevelManager> ALevelManager::Instance = nullptr;

// =======================
// コンストラクタ・初期化
// =======================

// コンストラクタ：Tickの有効化
ALevelManager::ALevelManager()
{
	// Tick処理を有効化
	PrimaryActorTick.bCanEverTick = true;
}

void ALevelManager::BeginPlay()
{
	Super::BeginPlay();
	Instance = this; // シングルトンとして自身を登録

	// コンポーネントの初期化
	InitializeComponents();
}

// 各マネージャーやコンポーネントを初期化
void ALevelManager::InitializeComponents()
{
	if (bInitialize) // 二重初期化防止
		return;

	// ColorManager を生成・初期化
	if (ColorManagerClass)
	{
		ColorManager = NewObject<UColorManager>(this, ColorManagerClass);
		ColorManager->Init();
	}

	// ScoreManager を生成・初期化
	if (ScoreManagerClass)
		ScoreManager = NewObject<UScoreManager>(this, ScoreManagerClass);
	if (ScoreManager)
		ScoreManager->Init();

	// SoundManager をコンポーネントから取得・初期化
	SoundManager = GetComponentByClass<USoundManager>();
	if (SoundManager)
	{
		SoundManager->Init();
		// デフォルトBGMを再生
		SoundManager->PlaySound("BGM", "Default", true, SoundManager->GetBGMVolume());
	}

	// UIManager を生成・初期化
	if (UIManagerClass)
		UIManager = NewObject<UUIManager>(this, UIManagerClass);
	if (UIManager)
	{
		UIManager->Init(this);
	}

	bInitialize = true;
}

// =======================
// Tick処理
// =======================

void ALevelManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	// 毎フレームで必要な処理があればここに記述
}

// =======================
// シングルトンインスタンス取得
// =======================

// GetInstance関数で、インスタンスが未設定の場合は初期化処理を強制する
ALevelManager* ALevelManager::GetInstance(UObject* WorldContext)
{
	if (!Instance.IsValid())
	{
		// WorldContextからWorldを取得
		UWorld* World = WorldContext ? WorldContext->GetWorld() : nullptr;
		if (!World)
			return nullptr;

		// 既存の LevelManager を検索
		for (TActorIterator<ALevelManager> It(World); It; ++It)
		{
			Instance = *It;
			Instance->InitializeComponents();  // 必要な初期化処理を行う
			return *It;
		}

		// 見つからなければ新規生成
		ALevelManager* NewInstance = World->SpawnActor<ALevelManager>();
		if (!Instance.IsValid())
			return nullptr;
		Instance = NewInstance;
		Instance->InitializeComponents();
	}

	return Instance.Get();
}

// =======================
// サウンド制御
// =======================

// サウンドを再生
void ALevelManager::PlaySound(FName WaveName, FName SoundName)
{
	if (!SoundManager)
	{
		UE_LOG(LogTemp, Warning, TEXT("SoundManager is null when trying to play sound."));
		return;
	}

	SoundManager->PlaySound(WaveName, SoundName);
}

// サウンドマネージャを取得（ISoundableインターフェースで返す）
TScriptInterface<ISoundable> ALevelManager::GetSoundManager() const
{
	return TScriptInterface<ISoundable>(SoundManager);
}

// =======================
// プレイヤーゴール処理
// =======================

// プレイヤーがゴール到達したときの処理
void ALevelManager::HandlePlayerGoalReached()
{
	if (!ScoreManager || !UIManager) return;

	// クリアタイムとランクを取得
	float ClearTime = ScoreManager->GetTime();
	EStageRank Rank = ScoreManager->EvaluateClearRank(GetWorld());

	// リザルト画面を表示
	UUserWidget* ResultWidget = UIManager->ShowResultWidget(ClearTime, Rank);

	// BGMを止め、ファンファーレを再生
	SoundManager->StopBGM();
	SoundManager->PlaySound("SE", "Fanfare");

	// UI入力専用モードにする場合はここで呼ぶ
	// PauseGameAndShowUI(ResultWidget);
}

// =======================
// UI専用の入力モードに切り替え
// =======================

// ゲームをポーズしてUIをフォーカス
void ALevelManager::PauseGameAndShowUI(UUserWidget* FocusWidget)
{
	UGameplayStatics::SetGamePaused(GetWorld(), true);

	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (!PC)
		return;

	// マウスカーソルを表示
	PC->bShowMouseCursor = true;

	// UI操作モードに切り替え
	FInputModeUIOnly InputMode;
	if (FocusWidget)
	{
		InputMode.SetWidgetToFocus(FocusWidget->TakeWidget());
	}
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	PC->SetInputMode(InputMode);
}
