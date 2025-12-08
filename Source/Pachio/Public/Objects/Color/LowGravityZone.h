// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Objects/Color/ColorReactiveObject.h"
#include "DataContainer/ColorTargetTypes.h"
#include "LowGravityZone.generated.h"

class UBoxComponent;
class UNiagaraSystem;
class UNiagaraComponent;

UCLASS()
class PACHIO_API ALowGravityZone : public AColorReactiveObject
{
	GENERATED_BODY()

public:
	/**
	 * コンストラクタ
	 * デフォルト値の設定および必要なコンポーネントを初期化する
	 */
	ALowGravityZone();

	/**
	 * 初期化処理
	 * ゾーン内の効果設定やNiagaraの初期化を行う
	 */
	virtual void Initialize() override;

private:
	/**
	 * ポストプロセスエフェクトの有効・無効を切り替える
	 *
	 * @param bEnable 有効にする場合は true、無効にする場合は false
	 */
	void SetPostProcessEffectEnabled(bool bEnable);

	/**
	 * オーバーラップ開始時の処理
	 * 対象がゾーン内に入った際に呼ばれる
	 *
	 * @param OverlappedComp このアクターのコリジョン
	 * @param OtherActor 侵入してきたアクター
	 * @param OtherComp 侵入したアクターのコリジョン
	 * @param OtherBodyIndex インデックス
	 * @param bFromSweep スイープによる検出か
	 * @param SweepResult ヒット情報
	 */
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);

	/**
	 * オーバーラップ終了時の処理
	 * 対象がゾーン外に出た際に呼ばれる
	 *
	 * @param OverlappedComp このアクターのコリジョン
	 * @param OtherActor 退出したアクター
	 * @param OtherComp 退出したアクターのコリジョン
	 * @param OtherBodyIndex インデックス
	 */
	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	/**
	 * 色反応処理
	 * 色の一致状態に応じて重力を軽減または通常状態に戻す
	 *
	 * @param InColor 適用された新しい色
	 */
	virtual void ApplyColorWithMatching(const FLinearColor& InColor) override;

private:
	/** ゾーン領域を定義するコリジョンボックス */
	UPROPERTY(EditAnywhere)
	UBoxComponent* ZoneBox;

	/** 現在ゾーン内に存在しているアクターの集合 */
	UPROPERTY()
	TSet<AActor*> OverlappingActors;

	/** 宇宙空間のようなエフェクトを表現するNiagaraシステム */
	UPROPERTY(EditAnywhere, Category = "Universe Effects")
	UNiagaraSystem* UniverseSystem;

	/** 重力のスケール値（1.0より小さいと重力が弱くなる） */
	UPROPERTY(EditAnywhere)
	float GravityScale;

	/** ジャンプ力の補正倍率 */
	UPROPERTY(EditAnywhere)
	float JumpBuff;

	/** 実際に再生されているNiagaraコンポーネント */
	UPROPERTY()
	UNiagaraComponent* UniverseEffect;

	/** 内部フラグ（デバッグや状態管理に使用） */
	bool bIsActive;

private:
	static constexpr float DEFAULT_GRAVITY_SCALE = 0.5f;
	static constexpr float DEFAULT_JUMP_BUFF = 1.f;
};
