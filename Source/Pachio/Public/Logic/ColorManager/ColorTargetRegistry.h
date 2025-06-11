// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "DataContainer/EffectMatchResult.h"
#include "DataContainer/ColorTargetType.h"
#include "ColorTargetRegistry.generated.h"

class IColorReactiveInterface;

UCLASS()
class PACHIO_API UColorTargetRegistry : public UObject
{
	GENERATED_BODY()
public:
    // 色付け対象を登録する関数
    void RegisterTarget(EColorTargetType Mode, TScriptInterface<IColorReactiveInterface> Target);
    void NotifyTargets(EColorTargetType Mode, const FLinearColor& Color);

private:
	//色に反応するオブジェクトに現在の色を通知
	UPROPERTY()
	TMap<EColorTargetType, FColorTargetInstanceArray> ColorResponseTargets;
};
