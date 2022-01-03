// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/image_effects/fish_eye_effect.h"
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
#include "pomdog/math/math.h"
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
    const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice,
    AssetManager& assets)
{
    samplerLinear = std::get<0>(graphicsDevice->CreateSamplerState(
        gpu::SamplerDescriptor::CreateLinearWrap()));

    auto inputLayout = gpu::InputLayoutHelper{}
                           .Float3()
                           .Float2();

    auto vertexShaderBuilder = assets.CreateBuilder<gpu::Shader>(gpu::ShaderPipelineStage::VertexShader);
    auto pixelShaderBuilder = assets.CreateBuilder<gpu::Shader>(gpu::ShaderPipelineStage::PixelShader);

#if defined(POMDOG_PLATFORM_WIN32) || \
    defined(POMDOG_PLATFORM_LINUX) || \
    defined(POMDOG_PLATFORM_MACOSX) || \
    defined(POMDOG_PLATFORM_EMSCRIPTEN)
    vertexShaderBuilder.SetGLSL(Builtin_GLSL_ScreenQuad_VS, std::strlen(Builtin_GLSL_ScreenQuad_VS));
    pixelShaderBuilder.SetGLSL(Builtin_GLSL_FishEye_PS, std::strlen(Builtin_GLSL_FishEye_PS));
#endif
#if defined(POMDOG_PLATFORM_WIN32)
    vertexShaderBuilder.SetHLSLPrecompiled(BuiltinHLSL_ScreenQuad_VS, sizeof(BuiltinHLSL_ScreenQuad_VS));
    pixelShaderBuilder.SetHLSLPrecompiled(BuiltinHLSL_FishEye_PS, sizeof(BuiltinHLSL_FishEye_PS));
#endif
#if defined(POMDOG_PLATFORM_MACOSX)
    vertexShaderBuilder.SetMetal(Builtin_Metal_ScreenQuad_VS, std::strlen(Builtin_Metal_ScreenQuad_VS), "ScreenQuadVS");
    pixelShaderBuilder.SetMetal(Builtin_Metal_FishEye_PS, std::strlen(Builtin_Metal_FishEye_PS), "FishEyePS");
#endif

    auto [vertexShader, vertexShaderErr] = vertexShaderBuilder.Build();
    if (vertexShaderErr != nullptr) {
        // FIXME: error handling
    }

    auto [pixelShader, pixelShaderErr] = pixelShaderBuilder.Build();
    if (pixelShaderErr != nullptr) {
        // FIXME: error handling
    }

    auto presentationParameters = graphicsDevice->GetPresentationParameters();

    std::unique_ptr<Error> pipelineStateErr;
    std::tie(pipelineState, pipelineStateErr) = assets.CreateBuilder<gpu::PipelineState>()
                                                    .SetRenderTargetViewFormat(presentationParameters.BackBufferFormat)
                                                    .SetDepthStencilViewFormat(presentationParameters.DepthStencilFormat)
                                                    .SetVertexShader(std::move(vertexShader))
                                                    .SetPixelShader(std::move(pixelShader))
                                                    .SetInputLayout(inputLayout.CreateInputLayout())
                                                    .SetPrimitiveTopology(gpu::PrimitiveTopology::TriangleList)
                                                    .SetBlendState(gpu::BlendDescriptor::CreateOpaque())
                                                    .SetDepthStencilState(gpu::DepthStencilDescriptor::CreateNone())
                                                    .SetConstantBufferBindSlot("ImageEffectConstants", 0)
                                                    .SetConstantBufferBindSlot("FishEyeBlock", 1)
                                                    .Build();
    if (pipelineStateErr != nullptr) {
        // FIXME: error handling
    }

    constantBufferFishEye = std::get<0>(graphicsDevice->CreateConstantBuffer(
        sizeof(FishEyeBlock),
        gpu::BufferUsage::Dynamic));

    SetStrength(0.04f);
}

void FishEyeEffect::SetStrength(float strength)
{
    strength_ = math::Clamp(strength, -0.5f, 0.5f);
}

void FishEyeEffect::UpdateGPUResources()
{
    FishEyeBlock block;
    block.Strength = strength_;

    POMDOG_ASSERT(constantBufferFishEye);
    constantBufferFishEye->SetData(0, gpu::MakeByteSpan(block));
}

void FishEyeEffect::Apply(
    gpu::CommandList& commandList,
    const std::shared_ptr<gpu::RenderTarget2D>& source,
    const std::shared_ptr<gpu::ConstantBuffer>& constantBuffer)
{
    POMDOG_ASSERT(source);
    POMDOG_ASSERT(constantBuffer);
    commandList.SetConstantBuffer(0, constantBuffer);
    commandList.SetConstantBuffer(1, constantBufferFishEye);
    commandList.SetSamplerState(0, samplerLinear);
    commandList.SetTexture(0, source);
    commandList.SetPipelineState(pipelineState);
}

} // namespace pomdog
