// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ColorManager.generated.h"

class IColorFilterInterface;

UENUM(BlueprintType)
enum class EColorMode : uint8
{
    Layer,
    Object,
    Background
};
UCLASS(Blueprintable)
class PACHIO_API UColorManager : public UActorComponent
{
    GENERATED_BODY()

public:
    void Init();

    UFUNCTION()
    void ApplyColor(FLinearColor NewColor);  // 引数を「値渡し」に変更

    void RegisterTarget(EColorMode Mode, TScriptInterface<IColorFilterInterface> Target);

    void SetMode(EColorMode Mode);

private:
    TMap<EColorMode, TArray<TScriptInterface<IColorFilterInterface>>> ColorTargets;

    // Optional: 現在のレイヤーターゲット（1つだけ）
    TScriptInterface<IColorFilterInterface> ActiveLayerTarget;

    FLinearColor CurrentColor;

    EColorMode Mode = EColorMode::Layer;
};