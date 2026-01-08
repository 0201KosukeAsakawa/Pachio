#include "RenderingExtModule.h"
#include "RenderingExtViewExtension.h"
#include "SceneViewExtension.h"
#include "Interfaces/IPluginManager.h"
#include "ShaderCore.h"

static TSharedPtr<FRenderingExtViewExtension, ESPMode::ThreadSafe> ViewExtension;

void FRenderingExtModule::StartupModule()
{
    const TSharedPtr<IPlugin> Plugin =
        IPluginManager::Get().FindPlugin(TEXT("RenderingExt"));

    check(Plugin.IsValid());

    const FString ShaderDir =
        FPaths::Combine(Plugin->GetBaseDir(), TEXT("Shaders"));

    AddShaderSourceDirectoryMapping(
        TEXT("/Plugin/RenderingExt"),
        ShaderDir
    );

    // ★ Engine 初期化後に登録
    FCoreDelegates::OnPostEngineInit.AddRaw(
        this,
        &FRenderingExtModule::OnPostEngineInit
    );
}

void FRenderingExtModule::OnPostEngineInit()
{
    ViewExtension =
        FSceneViewExtensions::NewExtension<FRenderingExtViewExtension>();

    // Debug RenderTarget を GameThread で一度だけ生成
    UTextureRenderTarget2D* DebugRT =
        NewObject<UTextureRenderTarget2D>(GetTransientPackage());

    // ★ フォーマットを明示的に PF_FloatRGBA に揃える
    DebugRT->InitCustomFormat(
        512,
        512,
        PF_FloatRGBA,
        false);

    DebugRT->ClearColor = FLinearColor::Black;

    // ★ UAV + RenderTargetable 両方必要
    DebugRT->bCanCreateUAV = true;

    DebugRT->UpdateResourceImmediate(true);

    // デバッグ中はGC防止
    DebugRT->AddToRoot();

    ViewExtension->SetDebugRenderTarget(DebugRT);
}

void FRenderingExtModule::ShutdownModule()
{
    ViewExtension.Reset();
}

IMPLEMENT_MODULE(FRenderingExtModule, RenderingExt)