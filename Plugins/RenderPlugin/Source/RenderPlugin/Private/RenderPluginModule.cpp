#include "RenderPluginModule.h"

#include "RenderPluginLog.h"
#include "RenderPluginSettings.h"

#include "ISettingsModule.h"
#include "Modules/ModuleManager.h"

IMPLEMENT_MODULE(FRenderPluginModule, RenderPlugin)

void FRenderPluginModule::StartupModule()
{
    UE_LOG(LogRenderPlugin, Log, TEXT("RenderPlugin Startup"));

    if (ISettingsModule* Settings =
        FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
    {
        Settings->RegisterSettings(
            "Project",
            "Plugins",
            "RenderPlugin",
            FText::FromString("Render Plugin"),
            FText::FromString("Settings for RenderPlugin"),
            GetMutableDefault<URenderPluginSettings>()
        );
    }
}

void FRenderPluginModule::ShutdownModule()
{
    if (ISettingsModule* Settings =
        FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
    {
        Settings->UnregisterSettings(
            "Project",
            "Plugins",
            "RenderPlugin"
        );
    }

    UE_LOG(LogRenderPlugin, Log, TEXT("RenderPlugin Shutdown"));
}