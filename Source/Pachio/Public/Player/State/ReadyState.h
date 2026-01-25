// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Player/PlayerStateComponent.h"
#include "ReadyState.generated.h"

/**
 * 
 */
UCLASS()
class PACHIO_API UReadyState : public UPlayerStateComponent
{
	GENERATED_BODY()
private:
	// ステートに入ったときの処理
	virtual bool OnEnter(APawn* Owner)override;

	// 毎フレームの更新処理（Tick の代わりに呼ばれる）
	virtual bool OnUpdate(float DeltaTime)override;

	// ステートから出るときの処理
	virtual bool OnExit(APawn* Owner)override;

	// スキル入力時の処理
	virtual bool OnSkill(const FInputActionValue& Input)override;
};
