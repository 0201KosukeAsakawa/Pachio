// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Color/ObjectColorComponent.h"
#include "DataContainer/ColorTargetTypes.h"
#include "ColorReactiveBeltConveyor.generated.h"

class UBoxComponent;
class UPhysicsCalculator;

/**
 * @brief 色によって挙動（ベルトの進行方向）が変化するベルトコンベアコンポーネント
 * 対象のアクター（UPhysicsCalculator を持つ）に力を加える
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PACHIO_API UColorReactiveBeltConveyorComponent : public UObjectColorComponent
{
	GENERATED_BODY()

public:
	/**
	 * @brief コンストラクタ。ベルトコンベアの初期設定を行う
	 */
	UColorReactiveBeltConveyorComponent();

	/**
	 * @brief 初期化処理（親クラスの初期化も呼び出す）
	 */
	virtual void Initialize() override;

	/**
	 * @brief コンポーネント登録時処理
	 *
	 * アクターにアタッチされた直後（エディタ・実行時とも）に呼ばれます。
	 * 必要であれば、この段階で依存コンポーネントの取得や初期設定を行います。
	 * @note BeginPlay よりも早いタイミングで呼び出されます。
	 */
	void OnRegister()override;

	/**
	 * @brief 毎フレーム呼ばれる処理
	 * ベルト上の物体に進行方向の力を加える
	 *
	 * @param DeltaTime フレーム間の経過時間
	 */
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	/**
	 * @brief 色の反応処理
	 * 色が一致した場合は進行方向へ、補色が一致した場合は逆方向へ切り替える
	 *
	 * @param NewColor 新しく適用された色
	 */
	virtual void ApplyColorWithMatching(const FLinearColor& NewColor) override;

	/**
	 * @brief オーバーラップ開始時の処理
	 * ベルト上に乗った対象を登録する
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
	 * @brief オーバーラップ終了時の処理
	 * ベルト上から離れた対象の登録を解除する
	 */
	UFUNCTION()
	void OnOverlapEnd(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex
	);

	/**
	 * @brief ビート検出時に呼ばれる処理
	 * ベルトの力やアニメーションを一時的に強調させる
	 */
	UFUNCTION()
	void OnBeatDetected();

private:
	/** ベルトコンベアの当たり判定用 Box コリジョン */
	UPROPERTY(VisibleAnywhere, Category = "BeltConveyor")
	TObjectPtr<UBoxComponent> BoxComponent;

	/** ベルトの基本進行方向（エディタから設定可能） */
	UPROPERTY(EditAnywhere, Category = "BeltConveyor|Movement")
	FVector BaseDirection;

	/** ベルトの基礎推進力（エディタ設定可） */
	UPROPERTY(EditAnywhere, Category = "BeltConveyor|Movement", meta = (ClampMin = "0.0"))
	float DefaultPower;

	/** 現在の実際の進行方向（色反応によって切り替わる） */
	UPROPERTY(VisibleAnywhere, Category = "BeltConveyor|Runtime")
	FVector CurrentDirection;

	/** ベルト上に存在する対象（物理的影響を受ける） */
	UPROPERTY()
	TArray<TObjectPtr<UPhysicsCalculator>> HitObjects;

	/** 色反応で方向を反転させるか */
	UPROPERTY(EditAnywhere, Category = "BeltConveyor|Behavior")
	bool bReverseOnComplementary;

	/** ローカルオフセットを使用するか */
	UPROPERTY(EditAnywhere, Category = "BeltConveyor|Behavior")
	bool bUseLocalOffset;

	/** 最も近い対象のみに作用させるか */
	UPROPERTY(EditAnywhere, Category = "BeltConveyor|Behavior")
	bool bAffectOnlyClosest;
};