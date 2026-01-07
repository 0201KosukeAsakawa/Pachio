#pragma once

#include "CoreMinimal.h"
#include "GlobalShader.h"
#include "ShaderParameterMacros.h"
#include "ShaderParameterStruct.h"
#include "ShaderPermutation.h"
#include "RenderGraphUtils.h"

/**
 * 最も基本的な Compute Shader のラッパクラス
 * UE5 では SHADER_USE_PARAMETER_STRUCT を使うのが必須。
 */
class FRenderingExtCS : public FGlobalShader
{
public:
    DECLARE_GLOBAL_SHADER(FRenderingExtCS);
    SHADER_USE_PARAMETER_STRUCT(FRenderingExtCS, FGlobalShader);

    /** Compute Shader に渡すパラメータ構造体 */
    BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
        SHADER_PARAMETER_RDG_TEXTURE_UAV(RWTexture2D<float4>, OutputTexture)
    END_SHADER_PARAMETER_STRUCT()
};