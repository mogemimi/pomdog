// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/image_effects/sepia_tone_effect.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/platform.h"
#include "pomdog/content/asset_builders/pipeline_state_builder.h"
#include "pomdog/content/asset_builders/shader_builder.h"
#include "pomdog/gpu/blend_descriptor.h"
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
#include "shaders/glsl.embedded/sepia_tone_ps.inc.h"
#endif
#if defined(POMDOG_PLATFORM_WIN32)
#include "shaders/hlsl.embedded/screen_quad_vs.inc.h"
#include "shaders/hlsl.embedded/sepia_tone_ps.inc.h"
#endif
#if defined(POMDOG_PLATFORM_MACOSX)
#include "shaders/metal.embedded/screen_quad_vs.inc.h"
#include "shaders/metal.embedded/sepia_tone_ps.inc.h"
#endif

} // namespace

SepiaToneEffect::SepiaToneEffect(
    const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice,
    AssetManager& assets)
{
    samplerLinear = std::get<0>(graphicsDevice->createSamplerState(
        gpu::SamplerDescriptor::createLinearWrap()));

    auto inputLayout = gpu::InputLayoutHelper{}
                           .addFloat3()
                           .addFloat2();

    auto vertexShaderBuilder = assets.createBuilder<gpu::Shader>(gpu::ShaderPipelineStage::VertexShader);
    auto pixelShaderBuilder = assets.createBuilder<gpu::Shader>(gpu::ShaderPipelineStage::PixelShader);

#if defined(POMDOG_PLATFORM_WIN32) || \
    defined(POMDOG_PLATFORM_LINUX) || \
    defined(POMDOG_PLATFORM_MACOSX) || \
    defined(POMDOG_PLATFORM_EMSCRIPTEN)
    vertexShaderBuilder.setGLSL(Builtin_GLSL_ScreenQuad_VS, std::strlen(Builtin_GLSL_ScreenQuad_VS));
    pixelShaderBuilder.setGLSL(Builtin_GLSL_SepiaTone_PS, std::strlen(Builtin_GLSL_SepiaTone_PS));
#endif
#if defined(POMDOG_PLATFORM_WIN32)
    vertexShaderBuilder.setHLSLPrecompiled(BuiltinHLSL_ScreenQuad_VS, sizeof(BuiltinHLSL_ScreenQuad_VS));
    pixelShaderBuilder.setHLSLPrecompiled(BuiltinHLSL_SepiaTone_PS, sizeof(BuiltinHLSL_SepiaTone_PS));
#endif
#if defined(POMDOG_PLATFORM_MACOSX)
    vertexShaderBuilder.setMetal(Builtin_Metal_ScreenQuad_VS, std::strlen(Builtin_Metal_ScreenQuad_VS), "ScreenQuadVS");
    pixelShaderBuilder.setMetal(Builtin_Metal_SepiaTone_PS, std::strlen(Builtin_Metal_SepiaTone_PS), "SepiaTonePS");
#endif

    auto [vertexShader, vertexShaderErr] = vertexShaderBuilder.build();
    if (vertexShaderErr != nullptr) {
        // FIXME: error handling
    }

    auto [pixelShader, pixelShaderErr] = pixelShaderBuilder.build();
    if (pixelShaderErr != nullptr) {
        // FIXME: error handling
    }

    auto presentationParameters = graphicsDevice->getPresentationParameters();

    std::unique_ptr<Error> pipelineStateErr;
    std::tie(pipelineState, pipelineStateErr) = assets.createBuilder<gpu::PipelineState>()
                                                    .setRenderTargetViewFormat(presentationParameters.backBufferFormat)
                                                    .setDepthStencilViewFormat(presentationParameters.depthStencilFormat)
                                                    .setVertexShader(std::move(vertexShader))
                                                    .setPixelShader(std::move(pixelShader))
                                                    .setInputLayout(inputLayout.createInputLayout())
                                                    .setPrimitiveTopology(gpu::PrimitiveTopology::TriangleList)
                                                    .setBlendState(gpu::BlendDescriptor::createOpaque())
                                                    .setDepthStencilState(gpu::DepthStencilDescriptor::createNone())
                                                    .setConstantBufferBindSlot("ImageEffectConstants", 0)
                                                    .build();
    if (pipelineStateErr != nullptr) {
        // FIXME: error handling
    }
}

void SepiaToneEffect::UpdateGPUResources()
{
}

void SepiaToneEffect::Apply(
    gpu::CommandList& commandList,
    const std::shared_ptr<gpu::RenderTarget2D>& source,
    const std::shared_ptr<gpu::ConstantBuffer>& constantBuffer)
{
    POMDOG_ASSERT(source);
    POMDOG_ASSERT(constantBuffer);
    commandList.setConstantBuffer(0, constantBuffer);
    commandList.setSamplerState(0, samplerLinear);
    commandList.setTexture(0, source);
    commandList.setPipelineState(pipelineState);
}

} // namespace pomdog
