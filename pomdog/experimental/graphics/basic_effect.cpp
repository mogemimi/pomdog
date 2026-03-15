// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/graphics/basic_effect.h"
#include "pomdog/content/asset_builders/pipeline_state_builder.h"
#include "pomdog/content/shader_loader.h"
#include "pomdog/gpu/graphics_device.h"
#include "pomdog/gpu/input_layout_helper.h"
#include "pomdog/gpu/pipeline_state.h"
#include "pomdog/gpu/presentation_parameters.h"
#include "pomdog/gpu/shader.h"
#include "pomdog/gpu/shader_language.h"
#include "pomdog/gpu/shader_pipeline_stage.h"

namespace pomdog::BasicEffect {
namespace {

// Built-in shaders
#include "shaders/glsl.embedded/basic_effect_ps.inc.h"
#include "shaders/glsl.embedded/basic_effect_vs.inc.h"
#include "shaders/hlsl.embedded/basic_effect.inc.h"
#include "shaders/metal.embedded/basic_effect.inc.h"

} // namespace

[[nodiscard]] PipelineStateBuilder
createBasicEffect(
    const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice,
    const BasicEffectDescription& desc)
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

    std::shared_ptr<gpu::Shader> vertexShader;
    std::shared_ptr<gpu::Shader> pixelShader;
    {
        std::unique_ptr<Error> shaderErr;
        const auto lang = graphicsDevice->getSupportedLanguage();
        if (lang == gpu::ShaderLanguage::GLSL) {
            std::tie(vertexShader, shaderErr) = createShaderFromSource(graphicsDevice, gpu::ShaderPipelineStage::VertexShader, sourceGLSLVS.data(), sourceGLSLVS.size(), "");
            if (shaderErr == nullptr) {
                std::tie(pixelShader, shaderErr) = createShaderFromSource(graphicsDevice, gpu::ShaderPipelineStage::PixelShader, sourceGLSLPS.data(), sourceGLSLPS.size(), "");
            }
        }
        else if (lang == gpu::ShaderLanguage::HLSL) {
            std::tie(vertexShader, shaderErr) = createShaderFromSource(graphicsDevice, gpu::ShaderPipelineStage::VertexShader, sourceHLSL.data(), sourceHLSL.size(), "BasicEffectVS");
            if (shaderErr == nullptr) {
                std::tie(pixelShader, shaderErr) = createShaderFromSource(graphicsDevice, gpu::ShaderPipelineStage::PixelShader, sourceHLSL.data(), sourceHLSL.size(), "BasicEffectPS");
            }
        }
        else if (lang == gpu::ShaderLanguage::Metal) {
            std::tie(vertexShader, shaderErr) = createShaderFromSource(graphicsDevice, gpu::ShaderPipelineStage::VertexShader, sourceMetal.data(), sourceMetal.size(), "BasicEffectVS");
            if (shaderErr == nullptr) {
                std::tie(pixelShader, shaderErr) = createShaderFromSource(graphicsDevice, gpu::ShaderPipelineStage::PixelShader, sourceMetal.data(), sourceMetal.size(), "BasicEffectPS");
            }
        }
        if (shaderErr != nullptr) {
            // FIXME: error handling
        }
    }

    auto builder = PipelineStateBuilder(graphicsDevice);
    if (vertexShader == nullptr || pixelShader == nullptr) {
        // FIXME: error handling
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
