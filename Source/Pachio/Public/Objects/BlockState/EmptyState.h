// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BlockState.h"
#include "EmptyState.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class PACHIO_API UEmptyState : public UBlockState
{
	GENERATED_BODY()
public:
	bool OnEnter(ABaseBlock*, UWorld*, UBlockDataContainer* ,const FString materialID = "None")override;

};
