#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/IDamageable.h"
#include "BaseBlock.generated.h"

// 前方宣言
class UBlockState;
class UBlockDataContainer;
class UBoxComponent;

// ABaseBlock クラスは AActor から派生したクラスで、IDamageable インターフェースを実装
UCLASS()
class PACHIO_API ABaseBlock : public AActor, public IDamageable
{
	GENERATED_BODY()

public:
	// このアクターのプロパティのデフォルト値を設定するコンストラクタ
	ABaseBlock();

protected:
	// ゲーム開始時またはスポーン時に呼ばれる
	virtual void BeginPlay() override;

public:
	// 初期化関数 (StateID, DropItemID, OptionalなMaterialID)
	void Init(FString stateID, FString dropItemID, const FString materialID = "None");

	// 毎フレーム呼ばれる関数
	virtual void Tick(float DeltaTime) override;

	// ダメージ処理のオーバーライド
	bool TakeDamage(FAttackData attackData, float damage = 0) override;

	// 状態を変更する関数
	void ChangeState(UBlockState* nextState);

	// DropItemIDを取得するゲッター関数
	FString GetDropItemID() const { return DropItemID; }

private:
	// ドロップアイテムのID
	UPROPERTY(EditAnywhere)
	FString DropItemID;

	// 現在のブロックの状態を識別するID
	UPROPERTY(EditAnywhere)
	FString StateID;

	// 現在のブロックの状態
	UPROPERTY()
	UBlockState* CurrentState;

	// 衝突コンポーネント（Box型）
	UPROPERTY()
	UBoxComponent* Collision;

	// 使用するBlockDataContainerのクラス
	UPROPERTY(EditAnywhere)
	TSubclassOf<UBlockDataContainer> ContainerClass;

	// 実際のBlockDataContainerのインスタンス
	TObjectPtr<UBlockDataContainer> Container;
};