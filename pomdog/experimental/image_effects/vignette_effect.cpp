// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/image_effects/vignette_effect.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/platform.h"
#include "pomdog/content/asset_builders/pipeline_state_builder.h"
#include "pomdog/content/asset_builders/shader_builder.h"
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
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstring>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
namespace {

// Built-in shaders
#if defined(POMDOG_PLATFORM_WIN32) || \
    defined(POMDOG_PLATFORM_LINUX) || \
    defined(POMDOG_PLATFORM_MACOSX) || \
    defined(POMDOG_PLATFORM_EMSCRIPTEN)
#include "shaders/glsl.embedded/screen_quad_vs.inc.h"
#include "shaders/glsl.embedded/vignette_ps.inc.h"
#endif
#if defined(POMDOG_PLATFORM_WIN32)
#include "shaders/hlsl.embedded/screen_quad_vs.inc.h"
#include "shaders/hlsl.embedded/vignette_ps.inc.h"
#endif
#if defined(POMDOG_PLATFORM_MACOSX)
#include "shaders/metal.embedded/screen_quad_vs.inc.h"
#include "shaders/metal.embedded/vignette_ps.inc.h"
#endif

struct VignetteBlock final {
    float Intensity;
    float x[15];
};

} // namespace

VignetteEffect::VignetteEffect(
    const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice,
    AssetManager& assets)
{
    samplerLinear = std::get<0>(graphicsDevice->createSamplerState(
        gpu::SamplerDescriptor::createLinearWrap()));

    auto inputLayout = gpu::InputLayoutHelper{}
                           .addFloat3()
                           .addFloat2();

    auto vertexShaderBuilder = assets.CreateBuilder<gpu::Shader>(gpu::ShaderPipelineStage::VertexShader);
    auto pixelShaderBuilder = assets.CreateBuilder<gpu::Shader>(gpu::ShaderPipelineStage::PixelShader);

#if defined(POMDOG_PLATFORM_WIN32) || \
    defined(POMDOG_PLATFORM_LINUX) || \
    defined(POMDOG_PLATFORM_MACOSX) || \
    defined(POMDOG_PLATFORM_EMSCRIPTEN)
    vertexShaderBuilder.SetGLSL(Builtin_GLSL_ScreenQuad_VS, std::strlen(Builtin_GLSL_ScreenQuad_VS));
    pixelShaderBuilder.SetGLSL(Builtin_GLSL_Vignette_PS, std::strlen(Builtin_GLSL_Vignette_PS));
#endif
#if defined(POMDOG_PLATFORM_WIN32)
    vertexShaderBuilder.SetHLSLPrecompiled(BuiltinHLSL_ScreenQuad_VS, sizeof(BuiltinHLSL_ScreenQuad_VS));
    pixelShaderBuilder.SetHLSLPrecompiled(BuiltinHLSL_Vignette_PS, sizeof(BuiltinHLSL_Vignette_PS));
#endif
#if defined(POMDOG_PLATFORM_MACOSX)
    vertexShaderBuilder.SetMetal(Builtin_Metal_ScreenQuad_VS, std::strlen(Builtin_Metal_ScreenQuad_VS), "ScreenQuadVS");
    pixelShaderBuilder.SetMetal(Builtin_Metal_Vignette_PS, std::strlen(Builtin_Metal_Vignette_PS), "VignettePS");
#endif

    auto [vertexShader, vertexShaderErr] = vertexShaderBuilder.Build();
    if (vertexShaderErr != nullptr) {
        // FIXME: error handling
    }

    auto [pixelShader, pixelShaderErr] = pixelShaderBuilder.Build();
    if (pixelShaderErr != nullptr) {
        // FIXME: error handling
    }

    auto presentationParameters = graphicsDevice->getPresentationParameters();

    std::unique_ptr<Error> pipelineStateErr;
    std::tie(pipelineState, pipelineStateErr) = assets.CreateBuilder<gpu::PipelineState>()
                                                    .SetRenderTargetViewFormat(presentationParameters.backBufferFormat)
                                                    .SetDepthStencilViewFormat(presentationParameters.depthStencilFormat)
                                                    .SetVertexShader(std::move(vertexShader))
                                                    .SetPixelShader(std::move(pixelShader))
                                                    .SetInputLayout(inputLayout.createInputLayout())
                                                    .SetPrimitiveTopology(gpu::PrimitiveTopology::TriangleList)
                                                    .SetBlendState(gpu::BlendDescriptor::createOpaque())
                                                    .SetDepthStencilState(gpu::DepthStencilDescriptor::createNone())
                                                    .SetConstantBufferBindSlot("ImageEffectConstants", 0)
                                                    .SetConstantBufferBindSlot("VignetteBlock", 1)
                                                    .Build();
    if (pipelineStateErr != nullptr) {
        // FIXME: error handling
    }

    constantBufferVignette = std::get<0>(graphicsDevice->createConstantBuffer(
        sizeof(VignetteBlock),
        gpu::BufferUsage::Dynamic));

    SetIntensity(0.5f);
}

void VignetteEffect::SetIntensity(float intensity)
{
    intensity_ = intensity;
}

void VignetteEffect::UpdateGPUResources()
{
    VignetteBlock block;
    block.Intensity = intensity_;

    constantBufferVignette->setData(0, gpu::makeByteSpan(block));
}

void VignetteEffect::Apply(
    gpu::CommandList& commandList,
    const std::shared_ptr<gpu::RenderTarget2D>& source,
    const std::shared_ptr<gpu::ConstantBuffer>& constantBuffer)
{
    POMDOG_ASSERT(source);
    POMDOG_ASSERT(constantBuffer);
    commandList.setConstantBuffer(0, constantBuffer);
    commandList.setConstantBuffer(1, constantBufferVignette);
    commandList.setSamplerState(0, samplerLinear);
    commandList.setTexture(0, source);
    commandList.setPipelineState(pipelineState);
}

} // namespace pomdog
