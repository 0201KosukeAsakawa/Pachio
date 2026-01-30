#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PPControllerActor.generated.h"

class APostProcessVolume;
class UMaterialInterface;

UCLASS()
class RENDERPLUGIN_API APPControllerActor : public AActor
{
    GENERATED_BODY()

public:
    APPControllerActor();

protected:
    virtual void BeginPlay() override;

public:
    UPROPERTY(EditAnywhere, Category = "RenderPlugin")
    APostProcessVolume* TargetVolume;

    UPROPERTY(EditAnywhere, Category = "RenderPlugin")
    UMaterialInterface* PostProcessMaterial;

    UPROPERTY(EditAnywhere, Category = "RenderPlugin")
    float BlendWeight = 1.0f;
};