#include "RenderingExtViewExtension.h"
#include "RenderingExtShader.h"
#include "RenderGraphUtils.h"
#include "SceneViewExtension.h"
#include "CanvasTypes.h"
#include "Engine/Canvas.h"
#include "Engine/Engine.h"
#include "PostProcess/PostProcessMaterialInputs.h"

FRenderingExtViewExtension::FRenderingExtViewExtension(
    const FAutoRegister& AutoRegister)
    : FSceneViewExtensionBase(AutoRegister)
{
}

void FRenderingExtViewExtension::PreRenderView_RenderThread(
    FRDGBuilder& GraphBuilder,
    FSceneView& View)
{
    if (!DebugRenderTarget)
        return;

    FTextureRenderTargetResource* RTResource =
        DebugRenderTarget->GetRenderTargetResource();

    if (!RTResource)
        return;

    FRHITexture* TextureRHI = RTResource->GetTextureRHI();
    if (!TextureRHI)
        return;

    // ===== ★ DebugRT のサイズを基準にする =====
    const FIntPoint Extent(
        DebugRenderTarget->SizeX,
        DebugRenderTarget->SizeY);

    if (Extent.X <= 0 || Extent.Y <= 0)
        return;

    // ===== RDG Work Texture（サイズ一致）=====
    FRDGTextureRef WorkTexture =
        GraphBuilder.CreateTexture(
            FRDGTextureDesc::Create2D(
                Extent,
                PF_FloatRGBA,
                FClearValueBinding::Black,
                TexCreate_ShaderResource |
                TexCreate_UAV |
                TexCreate_RenderTargetable),
            TEXT("RenderingExt_WorkRT"));

    // ===== Compute Shader =====
    TShaderMapRef<FRenderingExtCS> CS(
        GetGlobalShaderMap(View.GetFeatureLevel()));

    auto* Params =
        GraphBuilder.AllocParameters<FRenderingExtCS::FParameters>();

    Params->OutputTexture = GraphBuilder.CreateUAV(WorkTexture);

    FComputeShaderUtils::AddPass(
        GraphBuilder,
        RDG_EVENT_NAME("RenderingExt_DebugFill"),
        CS,
        Params,
        FIntVector(
            FMath::DivideAndRoundUp(Extent.X, 8),
            FMath::DivideAndRoundUp(Extent.Y, 8),
            1));

    // ===== External Texture =====
    FRDGTextureRef ExternalTexture =
        GraphBuilder.RegisterExternalTexture(
            CreateRenderTarget(
                TextureRHI,
                TEXT("RenderingExt_DebugRT")));

    // ===== SAFE COPY（サイズ完全一致）=====
    AddCopyTexturePass(
        GraphBuilder,
        WorkTexture,
        ExternalTexture);
}

void FRenderingExtViewExtension::AddRenderingExtPass(FRDGBuilder& GraphBuilder, FRDGTextureRef SceneColor)
{
}