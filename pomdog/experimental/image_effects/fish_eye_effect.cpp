// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/image_effects/fish_eye_effect.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/platform.h"
#include "pomdog/content/asset_builders/pipeline_state_builder.h"
#include "pomdog/content/asset_builders/shader_builder.h"
#include "pomdog/graphics/blend_description.h"
#include "pomdog/graphics/buffer_usage.h"
#include "pomdog/graphics/constant_buffer.h"
#include "pomdog/graphics/depth_stencil_description.h"
#include "pomdog/graphics/graphics_command_list.h"
#include "pomdog/graphics/graphics_device.h"
#include "pomdog/graphics/input_layout_helper.h"
#include "pomdog/graphics/pipeline_state.h"
#include "pomdog/graphics/presentation_parameters.h"
#include "pomdog/graphics/primitive_topology.h"
#include "pomdog/graphics/render_target2d.h"
#include "pomdog/graphics/sampler_state.h"
#include "pomdog/graphics/shader.h"
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
    const std::shared_ptr<GraphicsDevice>& graphicsDevice,
    AssetManager& assets)
{
    samplerLinear = std::get<0>(graphicsDevice->CreateSamplerState(
        SamplerDescription::CreateLinearWrap()));

    auto inputLayout = InputLayoutHelper{}
        .Float3().Float2();

    auto vertexShaderBuilder = assets.CreateBuilder<Shader>(ShaderPipelineStage::VertexShader);
    auto pixelShaderBuilder = assets.CreateBuilder<Shader>(ShaderPipelineStage::PixelShader);

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
    std::tie(pipelineState, pipelineStateErr) = assets.CreateBuilder<PipelineState>()
        .SetRenderTargetViewFormat(presentationParameters.BackBufferFormat)
        .SetDepthStencilViewFormat(presentationParameters.DepthStencilFormat)
        .SetVertexShader(std::move(vertexShader))
        .SetPixelShader(std::move(pixelShader))
        .SetInputLayout(inputLayout.CreateInputLayout())
        .SetPrimitiveTopology(PrimitiveTopology::TriangleList)
        .SetBlendState(BlendDescription::CreateOpaque())
        .SetDepthStencilState(DepthStencilDescription::CreateNone())
        .SetConstantBufferBindSlot("ImageEffectConstants", 0)
        .SetConstantBufferBindSlot("FishEyeBlock", 1)
        .Build();
    if (pipelineStateErr != nullptr) {
        // FIXME: error handling
    }

    constantBufferFishEye = std::get<0>(graphicsDevice->CreateConstantBuffer(
        sizeof(FishEyeBlock),
        BufferUsage::Dynamic));

    SetStrength(0.04f);
}

void FishEyeEffect::SetStrength(float strength)
{
    FishEyeBlock block;
    block.Strength = math::Clamp(strength, -0.5f, 0.5f);

    POMDOG_ASSERT(constantBufferFishEye);
    constantBufferFishEye->SetValue(std::move(block));
}

void FishEyeEffect::Apply(
    GraphicsCommandList& commandList,
    const std::shared_ptr<RenderTarget2D>& source,
    const std::shared_ptr<ConstantBuffer>& constantBuffer)
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
