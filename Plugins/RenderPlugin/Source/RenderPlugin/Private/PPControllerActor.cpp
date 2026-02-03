#include "PPControllerActor.h"

#include "Engine/PostProcessVolume.h"
#include "Materials/MaterialInterface.h"

APPControllerActor::APPControllerActor()
{
    PrimaryActorTick.bCanEverTick = false;
}

void APPControllerActor::BeginPlay()
{
    Super::BeginPlay();

    if (!TargetVolume || !PostProcessMaterial)
    {
        UE_LOG(LogTemp, Warning,
            TEXT("RenderPlugin: Volume or Material not set"));
        return;
    }

    TargetVolume->Settings.AddBlendable(
        PostProcessMaterial,
        BlendWeight
    );

    TargetVolume->Settings.WeightedBlendables.Array.Empty();

    TargetVolume->Settings.AddBlendable(
        PostProcessMaterial,
        1.0f
    );

    UE_LOG(LogTemp, Log,
        TEXT("RenderPlugin: PostProcess applied to dedicated volume"));
}