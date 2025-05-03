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

	/** グローバルから取得できるレベルマネージャー（シングルトン） */
	UFUNCTION(BlueprintCallable, Category = "Sound")
	static ALevelManager* GetInstance(UObject* WorldContext);

	/** サウンドの再生を指示する */
	void PlaySound(FName Category, FName CueName);

	/** ブロックデータ管理コンテナを取得 */
	UBlockDataContainer* GetBlockContainer() const { return BlockContainer; }

	/** アイテムデータ管理コンテナを取得 */
	UItemDataContainer* GetItemContainer() const { return ItemContainer; }

	/** 攻撃データ管理コンテナを取得 */
	UAttackDataContainer* GetAttackDataContainer() const { return AttackContainer; }

private:
	/** ステージ上のオブジェクトを生成（StageData をもとに） */
	void GenerateStage();

	/** ブロックを生成（BlockData をもとに） */
	void GenerateBlock();

private:
	/** サウンドマネージャークラス（Blueprintで指定） */
	UPROPERTY(EditAnywhere)
	TSubclassOf<USoundManager> SoundManagerClass;

	/** ブロックコンテナクラス（Blueprintで指定） */
	UPROPERTY(EditAnywhere)
	TSubclassOf<UBlockDataContainer> ContainerClass;

	/** オブジェクトマネージャークラス（Blueprintで指定） */
	UPROPERTY(EditAnywhere)
	TSubclassOf<UObjectManager> ObjectManagerClass;

	/** 攻撃データコンテナクラス（Blueprintで指定） */
	UPROPERTY(EditAnywhere)
	TSubclassOf<UAttackDataContainer> AttackContainerClass;

	/** アイテムデータコンテナクラス（Blueprintで指定） */
	UPROPERTY(EditAnywhere)
	TSubclassOf<UItemDataContainer> ItemContainerClass;

	/** ステージオブジェクト配置データテーブル */
	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UDataTable> StageData;

	/** ブロック配置データテーブル */
	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UDataTable> BlockData;

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

	/** シングルトンアクセス用のインスタンス */
	static TWeakObjectPtr<ALevelManager> Instance;
};
