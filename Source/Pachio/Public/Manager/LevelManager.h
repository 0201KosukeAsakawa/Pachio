#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LevelManager.generated.h"

// 前方宣言（依存クラスの参照を軽量化）
class USoundManager;
class UDataTable;
class UBlockDataContainer;
class UAttackDataContainer;
class UObjectManager;
class UItemDataContainer;
class UScoreManager;
class UUIManager;
class UEnemyDataContainer;
class UColorManager;

/**
 * ステージオブジェクト1つ分の配置情報（データテーブル用構造体）
 */
USTRUCT(BlueprintType)
struct FStageData : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere) FString ObjectName;      // 生成するオブジェクト名（Blueprint名など）
	UPROPERTY(EditAnywhere) FString MaterialName;    // 使用するマテリアル名
	UPROPERTY(EditAnywhere) float Location_X;        // 配置位置（X）
	UPROPERTY(EditAnywhere) float Location_Y;        // 配置位置（Y）
	UPROPERTY(EditAnywhere) float Location_Z;        // 配置位置（Z）
	UPROPERTY(EditAnywhere) float Rotate_X;          // 回転（X軸）
	UPROPERTY(EditAnywhere) float Rotate_Y;          // 回転（Y軸）
	UPROPERTY(EditAnywhere) float Rotate_Z;          // 回転（Z軸）
	UPROPERTY(EditAnywhere) float Scale_X;           // スケール（X軸）
	UPROPERTY(EditAnywhere) float Scale_Y;           // スケール（Y軸）
	UPROPERTY(EditAnywhere) float Scale_Z;           // スケール（Z軸）
};

/**
 * ステージ全体の生成・管理・シングルトンアクセスを提供するマネージャー
 */
UCLASS()
class PACHIO_API ALevelManager : public AActor
{
	GENERATED_BODY()

public:
	ALevelManager();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;	
	
	/** サウンドの再生を指示する */
	void PlaySound(FName Category, FName CueName);

	/** グローバルから取得できるレベルマネージャー（シングルトン） */
	UFUNCTION(BlueprintCallable, Category = "LevelManager")
	static ALevelManager* GetInstance(UObject* WorldContext);

	UFUNCTION(BlueprintCallable, Category = "LevelManager")
	inline int GetTime()const { return InGameTimer; }

	/** ブロックデータ管理コンテナを取得 */
	UFUNCTION(BlueprintCallable, Category = "LevelManager")
	inline UBlockDataContainer* GetBlockContainer() const { return BlockContainer; }

	/** アイテムデータ管理コンテナを取得 */
	UFUNCTION(BlueprintCallable, Category = "LevelManager")
	inline UItemDataContainer* GetItemContainer() const { return ItemContainer; }

	/** 攻撃データ管理コンテナを取得 */
	UFUNCTION(BlueprintCallable, Category = "LevelManager")
	inline UAttackDataContainer* GetAttackDataContainer() const { return AttackContainer; }

	UFUNCTION(BlueprintCallable, Category = "LevelManager")
	inline UScoreManager* GetScoreManager()const { return ScoreManager; }

	UFUNCTION(BlueprintCallable, Category = "LevelManager")
	inline UEnemyDataContainer* GetEnemyContainer()const { return EnemyContainer; }

	UFUNCTION(BlueprintCallable, Category = "LevelManager")
	inline UColorManager* GetColorManager()const { return ColorManager; }
	UFUNCTION(BlueprintCallable, Category = "LevelManager")
	inline UUIManager* GetUIManager()const { return UIManager; }

private:
	void InitializeComponents();

	/** ステージ上のオブジェクトを生成（StageData をもとに） */
	void GenerateStage();

	/** ブロックを生成（BlockData をもとに） */
	void GenerateBlock();

	void CountDown();

private:
	 bool bInitialize;

	UPROPERTY(EditAnywhere)
	float InGameTimer = 500.0f;

	FTimerHandle CountTimerHandle;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UEnemyDataContainer> EnemyContainerClass;

	/** サウンドマネージャークラス（Blueprintで指定） */
	UPROPERTY(EditAnywhere)
	TSubclassOf<USoundManager> SoundManagerClass;

	/** ブロックコンテナクラス（Blueprintで指定） */
	UPROPERTY(EditAnywhere)
	TSubclassOf<UBlockDataContainer> BlockContainerClass;

	/** オブジェクトマネージャークラス（Blueprintで指定） */
	UPROPERTY(EditAnywhere)
	TSubclassOf<UObjectManager> ObjectManagerClass;

	/** 攻撃データコンテナクラス（Blueprintで指定） */
	UPROPERTY(EditAnywhere)
	TSubclassOf<UAttackDataContainer> AttackContainerClass;

	/** アイテムデータコンテナクラス（Blueprintで指定） */
	UPROPERTY(EditAnywhere)
	TSubclassOf<UItemDataContainer> ItemContainerClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UScoreManager> ScoreManagerClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UUIManager> UIManagerClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UColorManager> ColorManagerClass;



	/** ステージオブジェクト配置データテーブル */
	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UDataTable> StageData;

	/** ブロック配置データテーブル */
	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UDataTable> BlockData;

	UPROPERTY()
	TObjectPtr<UEnemyDataContainer> EnemyContainer;

	/** 実行時に生成されたサウンドマネージャー */
	UPROPERTY()
	TObjectPtr<USoundManager> SoundManager;

	/** 実行時に生成されたブロックコンテナ */
	UPROPERTY()
	TObjectPtr<UBlockDataContainer> BlockContainer;

	/** 実行時に生成されたアイテムコンテナ */
	UPROPERTY()
	TObjectPtr<UItemDataContainer> ItemContainer;

	/** 実行時に生成されたオブジェクトマネージャー */
	UPROPERTY()
	TObjectPtr<UObjectManager> ObjectManager;

	/** 実行時に生成された攻撃データコンテナ */
	UPROPERTY()
	TObjectPtr<UAttackDataContainer> AttackContainer;

	UPROPERTY()
	TObjectPtr<UScoreManager> ScoreManager;

	UPROPERTY()
	TObjectPtr<UUIManager> UIManager;

	UPROPERTY()
	TObjectPtr<UColorManager> ColorManager;


	/** シングルトンアクセス用のインスタンス */
	static TWeakObjectPtr<ALevelManager> Instance;
};
