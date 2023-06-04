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

[[nodiscard]] AssetBuilders::Builder<gpu::PipelineState>
createBasicEffect(AssetManager& assets, const BasicEffectDescription& desc)
{
    gpu::InputLayoutHelper inputLayoutBuilder;
    inputLayoutBuilder.addFloat3();

    if (desc.lightingEnabled) {
        // NOTE: Add surface normal into the input layout.
        inputLayoutBuilder.addFloat3();
    }

    if (desc.textureEnabled) {
        // NOTE: Add texture coordinates into the input layout.
        inputLayoutBuilder.addFloat2();
    }
    else {
        // NOTE: Add vertex color into the input layout.
        inputLayoutBuilder.addFloat4();
    }

    // NOTE: Generate shader code for Metal
    std::string preprocessorMacroLines;
    if (desc.lightingEnabled) {
        preprocessorMacroLines += "#define LIGHTING_ENABLED 1\n";
    }
    else {
        preprocessorMacroLines += "#define LIGHTING_ENABLED 0\n";
    }
    if (desc.textureEnabled) {
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

    auto inputLayout = inputLayoutBuilder.createInputLayout();

    auto [vertexShader, vertexShaderErr] = assets.createBuilder<gpu::Shader>(gpu::ShaderPipelineStage::VertexShader)
                                               .setGLSL(sourceGLSLVS.data(), sourceGLSLVS.size())
                                               .setHLSL(sourceHLSL.data(), sourceHLSL.size(), "BasicEffectVS")
                                               .setMetal(sourceMetal.data(), sourceMetal.size(), "BasicEffectVS")
                                               .build();

    auto [pixelShader, pixelShaderErr] = assets.createBuilder<gpu::Shader>(gpu::ShaderPipelineStage::PixelShader)
                                             .setGLSL(sourceGLSLPS.data(), sourceGLSLPS.size())
                                             .setHLSL(sourceHLSL.data(), sourceHLSL.size(), "BasicEffectPS")
                                             .setMetal(sourceMetal.data(), sourceMetal.size(), "BasicEffectPS")
                                             .build();

    auto graphicsDevice = assets.getGraphicsDevice();

    auto builder = assets.createBuilder<gpu::PipelineState>();
    if (vertexShaderErr != nullptr) {
        builder.setError(std::move(vertexShaderErr));
        return builder;
    }

    if (pixelShaderErr != nullptr) {
        builder.setError(std::move(pixelShaderErr));
        return builder;
    }

    builder.setInputLayout(inputLayout);
    builder.setVertexShader(std::move(vertexShader));
    builder.setPixelShader(std::move(pixelShader));
    builder.setConstantBufferBindSlot("ModelConstantBuffer", 0);
    builder.setConstantBufferBindSlot("WorldConstantBuffer", 1);

    return builder;
}

} // namespace pomdog::BasicEffect
