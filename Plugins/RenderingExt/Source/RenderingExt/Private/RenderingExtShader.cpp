#include "RenderingExtShader.h"

IMPLEMENT_GLOBAL_SHADER(
    FRenderingExtCS,
    "/Plugin/RenderingExt/Private/RenderingExtShader.usf",
    "MainCS",
    SF_Compute
);