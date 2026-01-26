#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "RenderPluginSettings.generated.h"

UCLASS(Config = Game, DefaultConfig)
class RENDERPLUGIN_API URenderPluginSettings : public UObject
{
    GENERATED_BODY()

public:
    // 使用する PostProcessMaterial
    UPROPERTY(EditAnywhere, Config, Category = "PostProcess")
    TSoftObjectPtr<UMaterialInterface> PostProcessMaterial;

    // デフォルト強度
    UPROPERTY(EditAnywhere, Config, Category = "PostProcess")
    float DefaultBlendWeight = 1.0f;

    // デバッグ有効
    UPROPERTY(EditAnywhere, Config, Category = "Debug")
    bool bEnable = true;
};