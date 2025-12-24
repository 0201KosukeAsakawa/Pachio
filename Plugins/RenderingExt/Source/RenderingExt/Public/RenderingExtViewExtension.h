#pragma once

#include "CoreMinimal.h"
#include "SceneViewExtension.h"
#include "RenderGraphBuilder.h"
#include "RendererInterface.h"
#include "PostProcess/PostProcessMaterialInputs.h"
#include "Engine/TextureRenderTarget2D.h"


/**
 * FRenderingExtViewExtension
 * ----------------------------------------------
 * - Unreal Engine の描画パイプラインにフックする拡張クラス
 * - ViewFamily / View 単位の PreRender フック
 * - PostOpaqueRender により不透明物描画後のタイミングへ処理を追加
 * - RDG(Render Dependency Graph) パスを追加可能
 */
class FRenderingExtViewExtension final : public FSceneViewExtensionBase
{
public:
    FRenderingExtViewExtension(const FAutoRegister& AutoRegister);

    // ===== SceneViewExtensionBase overrides =====

    // 描画準備フェーズ（ほぼ何もしなくて OK）
    virtual void SetupViewFamily(FSceneViewFamily& InViewFamily) override {}
    virtual void SetupView(FSceneViewFamily& InViewFamily, FSceneView& InView) override {}
    virtual void BeginRenderViewFamily(FSceneViewFamily& InViewFamily) override {}

    virtual void PreRenderView_RenderThread(
        FRDGBuilder& GraphBuilder,
        FSceneView& View
    ) override;

    void SetDebugRenderTarget(UTextureRenderTarget2D* InRT)
    {
        DebugRenderTarget = InRT;
    }
    
    // ===== Custom hooks =====
    //void PostOpaqueRender(FPostOpaqueRenderParameters& Parameters);

private:
    // RDG パス追加
    void AddRenderingExtPass(FRDGBuilder& GraphBuilder, FRDGTextureRef SceneColor);

private:
    UTextureRenderTarget2D* DebugRenderTarget = nullptr;
    
};