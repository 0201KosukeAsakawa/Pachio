// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Player/PlayerStateComponent.h"
#include "DeadPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class PACHIO_API UDeadPlayerState : public UPlayerStateComponent
{
	GENERATED_BODY()
public:
	// ステートに入ったときの処理
	virtual bool OnEnter(APawn* Owner, UWorld* World)override;
};
