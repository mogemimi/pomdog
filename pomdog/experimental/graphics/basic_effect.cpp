// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/graphics/basic_effect.hpp"
#include "pomdog/content/asset_builders/pipeline_state_builder.hpp"
#include "pomdog/content/asset_builders/shader_builder.hpp"
#include "pomdog/content/asset_manager.hpp"
#include "pomdog/graphics/graphics_device.hpp"
#include "pomdog/graphics/input_layout_helper.hpp"
#include "pomdog/graphics/pipeline_state.hpp"
#include "pomdog/graphics/presentation_parameters.hpp"
#include "pomdog/graphics/shader.hpp"
#include "pomdog/graphics/shader_pipeline_stage.hpp"

namespace Pomdog::BasicEffect {
namespace {

// Built-in shaders
#include "shaders/glsl.embedded/basic_effect_ps.inc.hpp"
#include "shaders/glsl.embedded/basic_effect_vs.inc.hpp"
#include "shaders/hlsl.embedded/basic_effect.inc.hpp"
#include "shaders/metal.embedded/basic_effect.inc.hpp"

} // namespace

AssetBuilders::Builder<PipelineState>
CreateBasicEffect(AssetManager& assets, const BasicEffectDescription& desc)
{
    InputLayoutHelper inputLayoutBuilder;
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

    auto [vertexShader, vertexShaderErr] = assets.CreateBuilder<Shader>(ShaderPipelineStage::VertexShader)
        .SetGLSL(sourceGLSLVS.data(), sourceGLSLVS.size())
        .SetHLSL(sourceHLSL.data(), sourceHLSL.size(), "BasicEffectVS")
        .SetMetal(sourceMetal.data(), sourceMetal.size(), "BasicEffectVS")
        .Build();

    auto [pixelShader, pixelShaderErr] = assets.CreateBuilder<Shader>(ShaderPipelineStage::PixelShader)
        .SetGLSL(sourceGLSLPS.data(), sourceGLSLPS.size())
        .SetHLSL(sourceHLSL.data(), sourceHLSL.size(), "BasicEffectPS")
        .SetMetal(sourceMetal.data(), sourceMetal.size(), "BasicEffectPS")
        .Build();

    auto graphicsDevice = assets.GetGraphicsDevice();

    auto builder = assets.CreateBuilder<PipelineState>();
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

} // namespace Pomdog::BasicEffect
