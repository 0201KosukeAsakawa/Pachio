#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DataContainer/UIStruct.h"
#include "LevelManager.generated.h"

// 前方宣言（依存クラスの参照を軽量化）
class USoundManager;
class USaveManager;
class UScoreManager;
class UObjectManager;
class UColorManager;
class UUIManager;

class UWeatherEffectManager;
class UDataTable;
class ISoundable;
class UBlockDataContainer;
class UAttackDataContainer;
class UItemDataContainer;
class UEnemyDataContainer;

/**
 * レベル全体を統括・管理するクラス。
 *
 * ステージ遷移、スコア管理、サウンド、UI、敵データなど
 * ゲーム進行に関わる主要マネージャーの生成と制御を行う。
 */
UCLASS()
class PACHIO_API ALevelManager : public AActor
{
	GENERATED_BODY()

public:
	/** コンストラクタ：初期化フラグなどを設定 */
	ALevelManager();

protected:
	/** ゲーム開始時の初期化処理 */
	virtual void BeginPlay() override;

public:
	/**
	 * 次のステージ名を取得
	 *
	 * @return 遷移先ステージの名前
	 */
	UFUNCTION(BlueprintCallable, Category = "LevelManager")
	FString GetNextStageName() const { return NextStageName; }

	/**
	 * 毎フレーム呼ばれる処理
	 *
	 * @param DeltaTime 経過時間（秒）
	 */
	virtual void Tick(float DeltaTime) override;

	/** プレイヤーがゴールに到達した際の処理 */
	void HandlePlayerGoalReached();

	/**
	 * サウンドの再生を指示
	 *
	 * @param Category サウンドカテゴリ名
	 * @param CueName  再生するサウンドキュー名
	 */
	void PlaySound(FName Category, FName CueName);

	/**
	 * サウンドマネージャーを取得
	 *
	 * @return ISoundable インターフェースを実装したサウンド管理クラス
	 */
	UFUNCTION(BlueprintCallable, Category = "LevelManager")
	TScriptInterface<ISoundable> GetSoundManager() const;

	/**
	 * グローバルアクセス用のレベルマネージャーを取得（シングルトン）
	 *
	 * @param WorldContext 現在のワールドコンテキスト
	 * @return ALevelManager のインスタンス
	 */
	UFUNCTION(BlueprintCallable, Category = "LevelManager")
	static ALevelManager* GetInstance(UObject* WorldContext);

	/**
	 * スコアマネージャーを取得
	 *
	 * @return スコア管理クラス
	 */
	UFUNCTION(BlueprintCallable, Category = "LevelManager")
	inline UScoreManager* GetScoreManager() const { return ScoreManager; }

	/**
	 * カラーマネージャーを取得
	 *
	 * @return 色管理クラス
	 */
	UFUNCTION(BlueprintCallable, Category = "LevelManager")
	inline UColorManager* GetColorManager() const { return ColorManager; }

	/**
	 * UIマネージャーを取得
	 *
	 * @return UI管理クラス
	 */
	UFUNCTION(BlueprintCallable, Category = "LevelManager")
	inline UUIManager* GetUIManager() const { return UIManager; }

private:
	/**
	 * 各マネージャーや必要コンポーネントの初期化処理
	 */
	UFUNCTION(BlueprintCallable, Category = "LevelManager")
	void InitializeComponents();

	/**
	 * ゲームを一時停止し、指定のUIウィジェットを表示
	 *
	 * @param FocusWidget 表示対象のウィジェット
	 */
	void PauseGameAndShowUI(UUserWidget* FocusWidget);

private:
	/** 初期化済みかを示すフラグ */
	bool bInitialize;

	/** 現在のステージ名 */
	UPROPERTY(EditAnywhere)
	FString StageName;

	/** 次のステージ名 */
	UPROPERTY(EditAnywhere)
	FString NextStageName;

	/** スコアマネージャークラス（Blueprintで指定） */
	UPROPERTY(EditAnywhere)
	TSubclassOf<UScoreManager> ScoreManagerClass;

	/** UIマネージャークラス（Blueprintで指定） */
	UPROPERTY(EditAnywhere)
	TSubclassOf<UUIManager> UIManagerClass;

	/** カラーマネージャークラス（Blueprintで指定） */
	UPROPERTY(EditAnywhere)
	TSubclassOf<UColorManager> ColorManagerClass;

	/** 実行時に生成されたサウンドマネージャー */
	UPROPERTY()
	TObjectPtr<USoundManager> SoundManager;

	/** 実行時に生成されたスコアマネージャー */
	UPROPERTY()
	TObjectPtr<UScoreManager> ScoreManager;

	/** 実行時に生成されたUIマネージャー */
	UPROPERTY()
	TObjectPtr<UUIManager> UIManager;

	/** 実行時に生成されたカラーマネージャー */
	UPROPERTY()
	TObjectPtr<UColorManager> ColorManager;

	/** シングルトンアクセス用のレベルマネージャーインスタンス */
	static TWeakObjectPtr<ALevelManager> Instance;
};
