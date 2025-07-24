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

class UDataTable;

class UBlockDataContainer;
class UAttackDataContainer;
class UItemDataContainer;
class UEnemyDataContainer;

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

	void HandlePlayerGoalReached();
	/** サウンドの再生を指示する */
	void PlaySound(FName Category, FName CueName);

	/** グローバルから取得できるレベルマネージャー（シングルトン） */
	UFUNCTION(BlueprintCallable, Category = "LevelManager")
	static ALevelManager* GetInstance(UObject* WorldContext);

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
	UFUNCTION(BlueprintCallable, Category = "LevelManager")
	inline USoundManager* GetSoundManager()const { return SoundManager; }
private:
	UFUNCTION(BlueprintCallable, Category = "LevelManager")
	void InitializeComponents();

	void PauseGameAndShowUI(UUserWidget* FocusWidget);

private:
	 bool bInitialize;

	 UPROPERTY(EditAnywhere)
	 FString StageName;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UEnemyDataContainer> EnemyContainerClass;

	/** サウンドマネージャークラス（Blueprintで指定） */
	UPROPERTY(EditAnywhere)
	TSubclassOf<USoundManager> SoundManagerClass;



	UPROPERTY(EditAnywhere)
	TSubclassOf<UScoreManager> ScoreManagerClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UUIManager> UIManagerClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UColorManager> ColorManagerClass;

	UPROPERTY()
	TObjectPtr<UEnemyDataContainer> EnemyContainer;

	/** 実行時に生成されたサウンドマネージャー */
	UPROPERTY()
	TObjectPtr<USoundManager> SoundManager;

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
