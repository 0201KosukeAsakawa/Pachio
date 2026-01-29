#pragma once

#include "Modules/ModuleManager.h"

class FRenderPluginModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
};