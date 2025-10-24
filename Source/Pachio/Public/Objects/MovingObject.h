// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Color/ObjectColorComponent.h"
#include "DataContainer/ColorTargetTypes.h"
#include "MovingObject.generated.h"

class UBoxComponent;
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PACHIO_API UMoveOnColorComponent : public UObjectColorComponent
{
	GENERATED_BODY()

public:
	/**
	 * コンストラクタ
	 * デフォルト値の設定および必要なコンポーネントの初期化を行う
	 */
	UMoveOnColorComponent();

	/**
	 * 初期化処理
	 * オブジェクトの位置情報や関連するアクターの設定を行う
	 */
	virtual void Initialize() override;

	/**
	 * 毎フレーム呼ばれる処理
	 * 移動アニメーションを更新する
	 *
	 * @param DeltaTime 前フレームからの経過時間
	 */
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	/**
	 * 色反応処理
	 * 指定された色に応じて移動先を切り替える
	 *
	 * @param InColor 適用された新しい色
	 */
	virtual void ApplyColorWithMatching(const FLinearColor& InColor) override;

	/**
	 * オーバーラップ開始時の処理
	 * 対象が足場に乗った際に呼ばれる
	 *
	 * @param OverlappedComp このアクターのトリガーコンポーネント
	 * @param OtherActor 接触したアクター
	 * @param OtherComp 接触したアクターのコリジョン
	 * @param OtherBodyIndex インデックス
	 * @param bFromSweep スイープによる検出か
	 * @param SweepResult ヒット情報
	 */
	UFUNCTION()
	void OnFootBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	/**
	 * オーバーラップ終了時の処理
	 * 対象が足場から離れた際に呼ばれる
	 *
	 * @param OverlappedComp このアクターのトリガーコンポーネント
	 * @param OtherActor 離れたアクター
	 * @param OtherComp 離れたアクターのコリジョン
	 * @param OtherBodyIndex インデックス
	 */
	UFUNCTION()
	void OnFootEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

private:
	/** 現在移動中かどうか */
	bool bIsMoving = false;

	/** OFF状態時の位置 */
	UPROPERTY(EditAnywhere)
	FVector OffLocation;

	/** ON状態時の位置 */
	UPROPERTY(EditAnywhere)
	FVector OnLocation;

	/** 現在のターゲット位置 */
	FVector TargetLocation;

	/** 子オブジェクトの配列（連動移動対象） */
	UPROPERTY(EditAnywhere)
	TArray<AActor*> Child;

	/** 足場判定用のトリガー */
	UPROPERTY(EditAnywhere)
	UBoxComponent* FootTrigger;

	/** 現在足場に乗っているアクターのリスト */
	TArray<AActor*> AttachedActors;

	/** 移動開始位置 */
	FVector StartLocation;

	/** 移動にかける時間（秒） */
	UPROPERTY(EditAnywhere)
	float MoveDuration;

	/** 経過時間 */
	float ElapsedTime;

private:
	/** デフォルトの移動時間（1.0秒） */
	static constexpr float DEFAULT_DURATION = 1.0f;
};
