#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EnemyStateComponent.generated.h"

class AEnemyCharacter;
class UMaterialInterface;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PACHIO_API UEnemyStateComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// このコンポーネントのプロパティのデフォルト値を設定します。
	UEnemyStateComponent();

public:
	// 敵キャラクターがこの状態に入ったときに呼ばれます。
	// ここでは、状態に応じた初期化（ビジュアルの変更など）を行います。
	virtual bool OnEnter(AEnemyCharacter* Owner);

	// 状態ごとの挙動を毎フレーム更新します。
	// この関数で、移動や攻撃など、状態に基づくロジックを実行します。
	virtual bool OnUpdate(float DeltaTime);

	// 敵キャラクターが状態を終了する際に呼ばれます。
	// ここでは、状態から抜ける際に必要なリセットやクリーンアップを行います。
	virtual bool OnExit();

protected:
	// このコンポーネントがアタッチされている敵キャラクターの参照
	UPROPERTY()
	AEnemyCharacter* mOwner;

	/** 状態に応じて適用するマテリアル（ビジュアルの変更用） */
	// 状態によって敵の見た目を変更するために使用されます。
	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UMaterialInterface> NewMaterial;

};