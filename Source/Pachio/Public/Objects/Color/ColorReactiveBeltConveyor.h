// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Objects/Color/ColorReactiveObject.h"
#include "DataContainer/EffectMatchResult.h"
#include "ColorReactiveBeltConveyor.generated.h"

class UBoxComponent;
class UPhysicsCalculator;

/**
 * 色によって挙動（ベルトの進行方向）が変化するベルトコンベアクラス
 * 対象のアクター（UPhysicsCalculator を持つ）に力を加える
 */
UCLASS()
class PACHIO_API AColorReactiveBeltConveyor : public AColorReactiveObject
{
	GENERATED_BODY()

public:
	/**
	 * コンストラクタ。ベルトコンベアの初期設定を行う
	 */
	AColorReactiveBeltConveyor();

	/**
	 * 初期化処理（親クラスの初期化も呼び出す）
	 */
	virtual void Initialize() override;

	/**
	 * 毎フレーム呼ばれる処理
	 * ベルト上の物体に進行方向の力を加える
	 *
	 * @param DeltaTime フレーム間の経過時間
	 */
	virtual void Tick(float DeltaTime) override;

private:
	/**
	 * 色の反応処理
	 * 色が一致した場合は進行方向へ、補色が一致した場合は逆方向へ切り替える
	 *
	 * @param NewColor 新しく適用された色
	 */
	virtual void ApplyColorWithMatching(const FLinearColor& NewColor) override;

	/**
	 * オーバーラップ開始時に呼ばれる処理
	 * ベルト上に乗った対象を登録する
	 *
	 * @param OverlappedComp このコンベアのコリジョンコンポーネント
	 * @param OtherActor 重なったアクター
	 * @param OtherComp 重なったコンポーネント
	 * @param OtherBodyIndex 対象ボディのインデックス
	 * @param bFromSweep スウィープによるものか
	 * @param SweepResult スウィープの詳細結果
	 */
	UFUNCTION()
	void OnOverlapBegin(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	/**
	 * オーバーラップ終了時に呼ばれる処理
	 * ベルト上から離れた対象の登録を解除する
	 *
	 * @param OverlappedComp このコンベアのコリジョンコンポーネント
	 * @param OtherActor 離れたアクター
	 * @param OtherComp 離れたコンポーネント
	 * @param OtherBodyIndex 対象ボディのインデックス
	 */
	UFUNCTION()
	void OnOverlapEnd(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex
	);

	/**
	 * ビート検出時に呼ばれる処理
	 * ベルトの力やアニメーションを一時的に強調させる
	 */
	UFUNCTION()
	void OnBeatDetected();

private:
	/** ベルトコンベアの当たり判定用 Box コリジョン */
	UPROPERTY(VisibleAnywhere)
	UBoxComponent* BoxComponent;

	/** ベルトの基本進行方向（エディタから設定可能） */
	UPROPERTY(EditAnywhere)
	FVector direction;

	/** ベルトの基礎推進力（エディタ設定可） */
	UPROPERTY(EditAnywhere)
	float DefaultPower = 0;

	/** 現在の推進力（色やビートによって変化） */
	float CurrentPower = 0;

	/** 現在の実際の進行方向（色反応によって切り替わる） */
	FVector CurrentDirection;

	/** ベルト上に存在する対象（物理的影響を受ける） */
	UPROPERTY()
	TArray<UPhysicsCalculator*> hitObject;

	/** 色反応で方向を反転させるか */
	UPROPERTY(EditAnywhere)
	bool IsRevers = false;

	/** ローカルオフセットを使用するか */
	UPROPERTY(EditAnywhere)
	bool bUseLocalOffset = false;

	/** 最も近い対象のみに作用させるか */
	UPROPERTY(EditAnywhere)
	bool bOnlyClosest = false;
};
