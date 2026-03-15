// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/image_effects/fish_eye_effect.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/platform.h"
#include "pomdog/content/asset_builders/pipeline_state_builder.h"
#include "pomdog/content/shader_loader.h"
#include "pomdog/gpu/blend_descriptor.h"
#include "pomdog/gpu/buffer_usage.h"
#include "pomdog/gpu/command_list.h"
#include "pomdog/gpu/constant_buffer.h"
#include "pomdog/gpu/depth_stencil_descriptor.h"
#include "pomdog/gpu/graphics_device.h"
#include "pomdog/gpu/input_layout_helper.h"
#include "pomdog/gpu/pipeline_state.h"
#include "pomdog/gpu/presentation_parameters.h"
#include "pomdog/gpu/primitive_topology.h"
#include "pomdog/gpu/render_target2d.h"
#include "pomdog/gpu/sampler_state.h"
#include "pomdog/gpu/shader.h"
#include "pomdog/gpu/shader_language.h"
#include "pomdog/math/math_functions.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstring>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
namespace {

// Built-in shaders
#if defined(POMDOG_PLATFORM_WIN32) ||  \
    defined(POMDOG_PLATFORM_LINUX) ||  \
    defined(POMDOG_PLATFORM_MACOSX) || \
    defined(POMDOG_PLATFORM_EMSCRIPTEN)
#include "shaders/glsl.embedded/fish_eye_ps.inc.h"
#include "shaders/glsl.embedded/screen_quad_vs.inc.h"
#endif
#if defined(POMDOG_PLATFORM_WIN32)
#include "shaders/hlsl.embedded/fish_eye_ps.inc.h"
#include "shaders/hlsl.embedded/screen_quad_vs.inc.h"
#endif
#if defined(POMDOG_PLATFORM_MACOSX)
#include "shaders/metal.embedded/fish_eye_ps.inc.h"
#include "shaders/metal.embedded/screen_quad_vs.inc.h"
#endif

struct FishEyeBlock final {
    float Strength;
    float x[15];
};

} // namespace

FishEyeEffect::FishEyeEffect(
    const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice)
{
    samplerLinear = std::get<0>(graphicsDevice->createSamplerState(
        gpu::SamplerDescriptor::createLinearWrap()));

    auto inputLayout = gpu::InputLayoutHelper{}
                           .addFloat3()
                           .addFloat2();

    std::shared_ptr<gpu::Shader> vertexShader;
    std::shared_ptr<gpu::Shader> pixelShader;
    {
        std::unique_ptr<Error> shaderErr;
        const auto lang = graphicsDevice->getSupportedLanguage();
#if defined(POMDOG_PLATFORM_WIN32) ||  \
    defined(POMDOG_PLATFORM_LINUX) ||  \
    defined(POMDOG_PLATFORM_MACOSX) || \
    defined(POMDOG_PLATFORM_EMSCRIPTEN)
        if (lang == gpu::ShaderLanguage::GLSL) {
            std::tie(vertexShader, shaderErr) = createShaderFromSource(graphicsDevice, gpu::ShaderPipelineStage::VertexShader, Builtin_GLSL_ScreenQuad_VS, std::strlen(Builtin_GLSL_ScreenQuad_VS), "");
            if (shaderErr == nullptr) {
                std::tie(pixelShader, shaderErr) = createShaderFromSource(graphicsDevice, gpu::ShaderPipelineStage::PixelShader, Builtin_GLSL_FishEye_PS, std::strlen(Builtin_GLSL_FishEye_PS), "");
            }
        }
#endif
#if defined(POMDOG_PLATFORM_WIN32)
        if (lang == gpu::ShaderLanguage::HLSL) {
            std::tie(vertexShader, shaderErr) = createShaderFromBinary(graphicsDevice, gpu::ShaderPipelineStage::VertexShader, BuiltinHLSL_ScreenQuad_VS, sizeof(BuiltinHLSL_ScreenQuad_VS), "");
            if (shaderErr == nullptr) {
                std::tie(pixelShader, shaderErr) = createShaderFromBinary(graphicsDevice, gpu::ShaderPipelineStage::PixelShader, BuiltinHLSL_FishEye_PS, sizeof(BuiltinHLSL_FishEye_PS), "");
            }
        }
#endif
#if defined(POMDOG_PLATFORM_MACOSX)
        if (lang == gpu::ShaderLanguage::Metal) {
            std::tie(vertexShader, shaderErr) = createShaderFromSource(graphicsDevice, gpu::ShaderPipelineStage::VertexShader, Builtin_Metal_ScreenQuad_VS, std::strlen(Builtin_Metal_ScreenQuad_VS), "ScreenQuadVS");
            if (shaderErr == nullptr) {
                std::tie(pixelShader, shaderErr) = createShaderFromSource(graphicsDevice, gpu::ShaderPipelineStage::PixelShader, Builtin_Metal_FishEye_PS, std::strlen(Builtin_Metal_FishEye_PS), "FishEyePS");
            }
        }
#endif
        if (shaderErr != nullptr) {
            // FIXME: error handling
        }
    }

    auto presentationParameters = graphicsDevice->getPresentationParameters();

    auto pipelineStateBuilder = PipelineStateBuilder(graphicsDevice);
    pipelineStateBuilder.setRenderTargetViewFormat(presentationParameters.backBufferFormat);
    pipelineStateBuilder.setDepthStencilViewFormat(presentationParameters.depthStencilFormat);
    pipelineStateBuilder.setVertexShader(std::move(vertexShader));
    pipelineStateBuilder.setPixelShader(std::move(pixelShader));
    pipelineStateBuilder.setInputLayout(inputLayout.createInputLayout());
    pipelineStateBuilder.setPrimitiveTopology(gpu::PrimitiveTopology::TriangleList);
    pipelineStateBuilder.setBlendState(gpu::BlendDescriptor::createOpaque());
    pipelineStateBuilder.setDepthStencilState(gpu::DepthStencilDescriptor::createNone());
    pipelineStateBuilder.setConstantBufferBindSlot("ImageEffectConstants", 0);
    pipelineStateBuilder.setConstantBufferBindSlot("FishEyeBlock", 1);

    std::unique_ptr<Error> pipelineStateErr = nullptr;
    std::tie(pipelineState, pipelineStateErr) = pipelineStateBuilder.build();
    if (pipelineStateErr != nullptr) {
        // FIXME: error handling
    }

    constantBufferFishEye = std::get<0>(graphicsDevice->createConstantBuffer(
        sizeof(FishEyeBlock),
        gpu::BufferUsage::Dynamic));

    SetStrength(0.04f);
}

void FishEyeEffect::SetStrength(float strength)
{
    strength_ = math::clamp(strength, -0.5f, 0.5f);
}

void FishEyeEffect::UpdateGPUResources()
{
    FishEyeBlock block;
    block.Strength = strength_;

    POMDOG_ASSERT(constantBufferFishEye);
    constantBufferFishEye->setData(0, gpu::makeByteSpan(block));
}

void FishEyeEffect::Apply(
    gpu::CommandList& commandList,
    const std::shared_ptr<gpu::RenderTarget2D>& source,
    const std::shared_ptr<gpu::ConstantBuffer>& constantBuffer)
{
    POMDOG_ASSERT(source);
    POMDOG_ASSERT(constantBuffer);
    commandList.setConstantBuffer(0, constantBuffer);
    commandList.setConstantBuffer(1, constantBufferFishEye);
    commandList.setSamplerState(0, samplerLinear);
    commandList.setTexture(0, source);
    commandList.setPipelineState(pipelineState);
}

} // namespace pomdog
