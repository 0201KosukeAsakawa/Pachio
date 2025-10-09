// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Player/PlayerStateComponent.h"
#include "DeadPlayerState.generated.h"

class APlayerCharacter;
class URespawnComponent;

UCLASS()
class PACHIO_API UDeadPlayerState : public UPlayerStateComponent
{
	GENERATED_BODY()
public:
	// ステートに入ったときの処理
	virtual bool OnEnter(APawn* Owner, UWorld* World) override;

	// 毎フレームの更新処理（Tick の代わりに呼ばれる）
	virtual bool OnUpdate(float DeltaTime) override;

	// ステートから出るときの処理
	virtual bool OnExit(APawn* Owner) override;

private:
	// 経過時間
	float ElapsedTime = 0.f;

	// リスポーンまでの遅延時間（秒）
	const float RespawnDelay = 2.0f;

	// リスポーン済みフラグ
	bool bIsRespawned = false;

	// プレイヤーへの弱参照（GC安全）
	UPROPERTY()
	TWeakObjectPtr<APlayerCharacter> PlayerCharacter;

	// リスポーン用コンポーネントへの弱参照
	UPROPERTY()
	TWeakObjectPtr<URespawnComponent> RespawnComponent;
};
