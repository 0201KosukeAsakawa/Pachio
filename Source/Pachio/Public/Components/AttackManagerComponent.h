// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AttackManagerComponent.generated.h"

class UAttackComponent;

/**
 * 攻撃コンポーネントを一括で管理・制御するためのマネージャークラス。
 * 任意の攻撃IDと対応する攻撃コンポーネントを登録・取得できる。
 * 攻撃の種類が増えても柔軟に対応可能なように設計されている。
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PACHIO_API UAttackManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// コンストラクタ：このコンポーネントのデフォルト設定を初期化
	UAttackManagerComponent();

	void Init(UWorld*);

	/**
	 * 任意の攻撃IDと対応する攻撃コンポーネントクラスを登録する。
	 *
	 * @param AttackID 登録したい攻撃の識別名（例："Stomp", "Upper"など）
	 * @param AttackComponent 登録する攻撃コンポーネントのクラス型（TSubclassOf）
	 * @return 成功時 true、すでに存在するか無効な場合は false
	 */
	UFUNCTION(BlueprintCallable)
	bool RegisterAttackComponent(FName AttackID);


	UFUNCTION(BlueprintCallable)
	bool ResetMap();

	/**
	 * 登録されている攻撃IDに対応する攻撃コンポーネントクラスを取得する。
	 *
	 * @param AttackID 取得したい攻撃の識別名
	 * @return 対応する攻撃コンポーネントクラス。存在しない場合は nullptr
	 */
	 UAttackComponent* GetAttack(FName AttackID);

private:
	// 攻撃IDと対応する攻撃コンポーネントクラスを格納するマップ
	UPROPERTY()
	TMap<FName,UAttackComponent*> AttackMap;
	UPROPERTY()
	UWorld* pWorld;
};