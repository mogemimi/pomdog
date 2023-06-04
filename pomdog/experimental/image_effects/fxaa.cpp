// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/image_effects/fxaa.h"
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
#include "shaders/glsl.embedded/fxaa_ps.inc.h"
#include "shaders/glsl.embedded/fxaa_vs.inc.h"
#endif
#if defined(POMDOG_PLATFORM_WIN32)
#include "shaders/hlsl.embedded/fxaa_ps.inc.h"
#include "shaders/hlsl.embedded/fxaa_vs.inc.h"
#endif
#if defined(POMDOG_PLATFORM_MACOSX)
#include "shaders/metal.embedded/fxaa.inc.h"
#endif

} // namespace

FXAA::FXAA(
    const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice,
    AssetManager& assets)
{
    samplerLinear = std::get<0>(graphicsDevice->createSamplerState(
        gpu::SamplerDescriptor::createLinearClamp()));

    auto inputLayout = gpu::InputLayoutHelper{}
                           .addFloat3()
                           .addFloat2();

    auto vertexShaderBuilder = assets.createBuilder<gpu::Shader>(gpu::ShaderPipelineStage::VertexShader);
    auto pixelShaderBuilder = assets.createBuilder<gpu::Shader>(gpu::ShaderPipelineStage::PixelShader);

#if defined(POMDOG_PLATFORM_WIN32) || \
    defined(POMDOG_PLATFORM_LINUX) || \
    defined(POMDOG_PLATFORM_MACOSX) || \
    defined(POMDOG_PLATFORM_EMSCRIPTEN)
    vertexShaderBuilder.setGLSL(Builtin_GLSL_FXAA_VS, std::strlen(Builtin_GLSL_FXAA_VS));
    pixelShaderBuilder.setGLSL(Builtin_GLSL_FXAA_PS, std::strlen(Builtin_GLSL_FXAA_PS));
#endif
#if defined(POMDOG_PLATFORM_WIN32)
    vertexShaderBuilder.setHLSLPrecompiled(BuiltinHLSL_FXAA_VS, sizeof(BuiltinHLSL_FXAA_VS));
    pixelShaderBuilder.setHLSLPrecompiled(BuiltinHLSL_FXAA_PS, sizeof(BuiltinHLSL_FXAA_PS));
#endif
#if defined(POMDOG_PLATFORM_MACOSX)
    vertexShaderBuilder.setMetal(Builtin_Metal_FXAA, std::strlen(Builtin_Metal_FXAA), "FxaaVS");
    pixelShaderBuilder.setMetal(Builtin_Metal_FXAA, std::strlen(Builtin_Metal_FXAA), "FxaaPS");
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

void FXAA::UpdateGPUResources()
{
}

void FXAA::Apply(
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
