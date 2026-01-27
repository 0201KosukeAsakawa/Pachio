#include "PPControllerActor.h"

#include "Engine/PostProcessVolume.h"
#include "Materials/MaterialInterface.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Engine/World.h"
#include "EngineUtils.h"

#include "RenderPluginSettings.h"
#include "RenderPluginLog.h"

APPControllerActor::APPControllerActor()
{
    PrimaryActorTick.bCanEverTick = false;
}

void APPControllerActor::BeginPlay()
{
    //Super::BeginPlay();

    const URenderPluginSettings* Settings =
        GetDefault<URenderPluginSettings>();

    if (!Settings || !Settings->bEnable)
    {
        UE_LOG(LogRenderPlugin, Warning,
            TEXT("RenderPlugin disabled by settings"));
        return;
    }

    APostProcessVolume* Volume =
        TargetVolume ? TargetVolume : FindGlobalVolume();

    if (!Volume)
    {
        UE_LOG(LogRenderPlugin, Error,
            TEXT("No PostProcessVolume found"));
        return;
    }

    if (!SetupMaterial(Volume))
    {
        UE_LOG(LogRenderPlugin, Error,
            TEXT("Failed to setup PostProcessMaterial"));
        return;
    }

    // 実際に変更（テスト）
    MID->SetVectorParameterValue(
        TEXT("Color"),
        FLinearColor::Red
    );

    UE_LOG(LogRenderPlugin, Log,
        TEXT("PostProcess modified successfully"));
}

APostProcessVolume* APPControllerActor::FindGlobalVolume() const
{
    for (TActorIterator<APostProcessVolume> It(GetWorld()); It; ++It)
    {
        if (It->bUnbound)
        {
            UE_LOG(LogRenderPlugin, Log,
                TEXT("Found Unbound PostProcessVolume: %s"),
                *It->GetName());
            return *It;
        }
    }
    return nullptr;
}

bool APPControllerActor::SetupMaterial(APostProcessVolume* Volume)
{
    if (!Volume)
    {
        return false;
    }

    auto& Blendables = Volume->Settings.WeightedBlendables.Array;

    FWeightedBlendable* TargetBlendable = nullptr;

    for (auto& B : Blendables)
    {
        if (Cast<UMaterialInterface>(B.Object))
        {
            TargetBlendable = &B;
            break;
        }
    }

    if (!TargetBlendable)
    {
        UE_LOG(LogRenderPlugin, Error,
            TEXT("No PostProcessMaterial in Volume"));
        return false;
    }

    UMaterialInterface* BaseMat =
        Cast<UMaterialInterface>(TargetBlendable->Object);

    if (!BaseMat)
    {
        return false;
    }

    MID = UMaterialInstanceDynamic::Create(BaseMat, this);
    if (!MID)
    {
        return false;
    }

    // 差し替え
    TargetBlendable->Object = MID;

    return true;
}