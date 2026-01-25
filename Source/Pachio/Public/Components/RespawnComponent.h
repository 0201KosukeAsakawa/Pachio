// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RespawnComponent.generated.h"


/**
 * @class URespawnComponent
 * @brief アクターのリスポーン（再出現）を管理するコンポーネント。
 *
 * アクター（プレイヤーや敵など）の初期位置を記録し、死亡・ミス・リセット時などに
 * 指定された位置へ再配置する処理を提供します。
 *
 * @details
 * - BeginPlay時に初期位置を自動で記録します。
 * - BlueprintやC++から `RespawnOwner()` を呼ぶことで復帰可能です。
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PACHIO_API URespawnComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	/** コンストラクタ（デフォルト値の初期化） */
	URespawnComponent();

protected:
	/** ゲーム開始時に呼ばれる初期化処理 */
	virtual void BeginPlay() override;

public:
	/** 毎フレームの更新処理（特定条件でリスポーン監視などに使用可能） */
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/**
	 * アクター（Owner）を初期位置に戻す
	 *
	 * @note 通常は死亡やリセット時に呼び出されます。
	 * @warning Ownerがnullptrの場合は処理されません。
	 */
	UFUNCTION(BlueprintCallable, Category = "Respawn")
	void RespawnOwner();

	/**
	 * 現在のアクター位置を新しいリスポーン地点として登録する
	 *
	 * @param newPosition 新しいリスポーン座標
	 */
	UFUNCTION(BlueprintCallable, Category = "Respawn")
	void UpdateRespawnPosition(FVector newPosition);

private:
	/** 記録されたリスポーン位置（初期位置） */
	FVector respawnPosition;
};
