// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#include "Pomdog/Experimental/ImageEffects/FishEyeEffect.hpp"
#include "Pomdog/Basic/ConditionalCompilation.hpp"
#include "Pomdog/Basic/Platform.hpp"
#include "Pomdog/Content/AssetBuilders/PipelineStateBuilder.hpp"
#include "Pomdog/Content/AssetBuilders/ShaderBuilder.hpp"
#include "Pomdog/Graphics/BlendDescription.hpp"
#include "Pomdog/Graphics/BufferUsage.hpp"
#include "Pomdog/Graphics/ConstantBuffer.hpp"
#include "Pomdog/Graphics/DepthStencilDescription.hpp"
#include "Pomdog/Graphics/GraphicsCommandList.hpp"
#include "Pomdog/Graphics/GraphicsDevice.hpp"
#include "Pomdog/Graphics/InputLayoutHelper.hpp"
#include "Pomdog/Graphics/PipelineState.hpp"
#include "Pomdog/Graphics/PresentationParameters.hpp"
#include "Pomdog/Graphics/PrimitiveTopology.hpp"
#include "Pomdog/Graphics/RenderTarget2D.hpp"
#include "Pomdog/Graphics/SamplerState.hpp"
#include "Pomdog/Graphics/Shader.hpp"
#include "Pomdog/Math/Math.hpp"
#include "Pomdog/Utility/Assert.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstring>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog {
namespace {

// Built-in shaders
#if defined(POMDOG_PLATFORM_WIN32) || \
    defined(POMDOG_PLATFORM_LINUX) || \
    defined(POMDOG_PLATFORM_MACOSX) || \
    defined(POMDOG_PLATFORM_EMSCRIPTEN)
#include "Shaders/GLSL.Embedded/FishEye_PS.inc.hpp"
#include "Shaders/GLSL.Embedded/ScreenQuad_VS.inc.hpp"
#endif
#if defined(POMDOG_PLATFORM_WIN32)
#include "Shaders/HLSL.Embedded/FishEye_PS.inc.hpp"
#include "Shaders/HLSL.Embedded/ScreenQuad_VS.inc.hpp"
#endif
#if defined(POMDOG_PLATFORM_MACOSX)
#include "Shaders/Metal.Embedded/FishEye_PS.inc.hpp"
#include "Shaders/Metal.Embedded/ScreenQuad_VS.inc.hpp"
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
    block.Strength = Math::Clamp(strength, -0.5f, 0.5f);

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

} // namespace Pomdog
