#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PPControllerActor.generated.h"

class APostProcessVolume;
class UMaterialInstanceDynamic;

UCLASS()
class RENDERPLUGIN_API APPControllerActor : public AActor
{
    GENERATED_BODY()

public:
    APPControllerActor();

protected:
    virtual void BeginPlay() override;

public:
    // デバッグ用（未指定なら自動探索）
    UPROPERTY(EditAnywhere, Category = "PostProcess")
    APostProcessVolume* TargetVolume = nullptr;

private:
    // GC防止
    UPROPERTY()
    UMaterialInstanceDynamic* MID = nullptr;

    // 内部処理
    APostProcessVolume* FindGlobalVolume() const;
    bool SetupMaterial(APostProcessVolume* Volume);
};