#pragma once

#include "Modules/ModuleManager.h"

class FRenderingExtViewExtension;

class FRenderingExtModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;

private:
    void OnPostEngineInit();

    TSharedPtr<FRenderingExtViewExtension, ESPMode::ThreadSafe> ViewExtension;
};