// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/graphics/basic_effect.h"
#include "pomdog/content/asset_builders/pipeline_state_builder.h"
#include "pomdog/content/asset_builders/shader_builder.h"
#include "pomdog/content/asset_manager.h"
#include "pomdog/gpu/graphics_device.h"
#include "pomdog/gpu/input_layout_helper.h"
#include "pomdog/gpu/pipeline_state.h"
#include "pomdog/gpu/presentation_parameters.h"
#include "pomdog/gpu/shader.h"
#include "pomdog/gpu/shader_pipeline_stage.h"

namespace pomdog::BasicEffect {
namespace {

// Built-in shaders
#include "shaders/glsl.embedded/basic_effect_ps.inc.h"
#include "shaders/glsl.embedded/basic_effect_vs.inc.h"
#include "shaders/hlsl.embedded/basic_effect.inc.h"
#include "shaders/metal.embedded/basic_effect.inc.h"

} // namespace

AssetBuilders::Builder<gpu::PipelineState>
CreateBasicEffect(AssetManager& assets, const BasicEffectDescription& desc)
{
    gpu::InputLayoutHelper inputLayoutBuilder;
    inputLayoutBuilder.Float3();

    if (desc.LightingEnabled) {
        // NOTE: Add surface normal into the input layout.
        inputLayoutBuilder.Float3();
    }

    if (desc.TextureEnabled) {
        // NOTE: Add texture coordinates into the input layout.
        inputLayoutBuilder.Float2();
    }
    else {
        // NOTE: Add vertex color into the input layout.
        inputLayoutBuilder.Float4();
    }

    // NOTE: Generate shader code for Metal
    std::string preprocessorMacroLines;
    if (desc.LightingEnabled) {
        preprocessorMacroLines += "#define LIGHTING_ENABLED 1\n";
    }
    else {
        preprocessorMacroLines += "#define LIGHTING_ENABLED 0\n";
    }
    if (desc.TextureEnabled) {
        preprocessorMacroLines += "#define TEXTURE_ENABLED 1\n";
        preprocessorMacroLines += "#define VERTEX_COLOR_ENABLED 0\n";
    }
    else {
        preprocessorMacroLines += "#define TEXTURE_ENABLED 0\n";
        preprocessorMacroLines += "#define VERTEX_COLOR_ENABLED 1\n";
    }

    std::string sourceHLSL = preprocessorMacroLines + BuiltinHLSL_BasicEffect;
    std::string sourceMetal = preprocessorMacroLines + Builtin_Metal_BasicEffect;
    std::string sourceGLSLVS = "#version 330\n" + preprocessorMacroLines + Builtin_GLSL_BasicEffect_VS;
    std::string sourceGLSLPS = "#version 330\n" + preprocessorMacroLines + Builtin_GLSL_BasicEffect_PS;

    auto inputLayout = inputLayoutBuilder.CreateInputLayout();

    auto [vertexShader, vertexShaderErr] = assets.CreateBuilder<gpu::Shader>(gpu::ShaderPipelineStage::VertexShader)
                                               .SetGLSL(sourceGLSLVS.data(), sourceGLSLVS.size())
                                               .SetHLSL(sourceHLSL.data(), sourceHLSL.size(), "BasicEffectVS")
                                               .SetMetal(sourceMetal.data(), sourceMetal.size(), "BasicEffectVS")
                                               .Build();

    auto [pixelShader, pixelShaderErr] = assets.CreateBuilder<gpu::Shader>(gpu::ShaderPipelineStage::PixelShader)
                                             .SetGLSL(sourceGLSLPS.data(), sourceGLSLPS.size())
                                             .SetHLSL(sourceHLSL.data(), sourceHLSL.size(), "BasicEffectPS")
                                             .SetMetal(sourceMetal.data(), sourceMetal.size(), "BasicEffectPS")
                                             .Build();

    auto graphicsDevice = assets.GetGraphicsDevice();

    auto builder = assets.CreateBuilder<gpu::PipelineState>();
    if (vertexShaderErr != nullptr) {
        builder.SetError(std::move(vertexShaderErr));
        return builder;
    }

    if (pixelShaderErr != nullptr) {
        builder.SetError(std::move(pixelShaderErr));
        return builder;
    }

    builder.SetInputLayout(inputLayout);
    builder.SetVertexShader(std::move(vertexShader));
    builder.SetPixelShader(std::move(pixelShader));
    builder.SetConstantBufferBindSlot("ModelConstantBuffer", 0);
    builder.SetConstantBufferBindSlot("WorldConstantBuffer", 1);

    return builder;
}

} // namespace pomdog::BasicEffect
