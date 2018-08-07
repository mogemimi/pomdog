// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include "FishEyeEffect.hpp"
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
#include "Pomdog/Graphics/RenderTarget2D.hpp"
#include "Pomdog/Graphics/SamplerState.hpp"
#include "Pomdog/Graphics/Shader.hpp"
#include "Pomdog/Math/MathHelper.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include <cstring>

namespace Pomdog {
namespace {

// Built-in shaders
#include "Shaders/GLSL.Embedded/ScreenQuad_VS.inc.hpp"
#include "Shaders/GLSL.Embedded/FishEye_PS.inc.hpp"
#include "Shaders/HLSL.Embedded/ScreenQuad_VS.inc.hpp"
#include "Shaders/HLSL.Embedded/FishEye_PS.inc.hpp"
#include "Shaders/Metal.Embedded/ScreenQuad_VS.inc.hpp"
#include "Shaders/Metal.Embedded/FishEye_PS.inc.hpp"

struct FishEyeBlock {
    float Strength;
};

} // unnamed namespace

FishEyeEffect::FishEyeEffect(
    const std::shared_ptr<GraphicsDevice>& graphicsDevice,
    AssetManager & assets)
{
    samplerLinear = std::make_shared<SamplerState>(graphicsDevice,
        SamplerDescription::CreateLinearWrap());

    auto inputLayout = InputLayoutHelper{}
        .Float3().Float2();

    auto vertexShader = assets.CreateBuilder<Shader>(ShaderPipelineStage::VertexShader)
        .SetGLSL(Builtin_GLSL_ScreenQuad_VS, std::strlen(Builtin_GLSL_ScreenQuad_VS))
        .SetHLSLPrecompiled(BuiltinHLSL_ScreenQuad_VS, sizeof(BuiltinHLSL_ScreenQuad_VS))
        .SetMetal(Builtin_Metal_ScreenQuad_VS, std::strlen(Builtin_Metal_ScreenQuad_VS), "ScreenQuadVS");

    auto pixelShader = assets.CreateBuilder<Shader>(ShaderPipelineStage::PixelShader)
        .SetGLSL(Builtin_GLSL_FishEye_PS, std::strlen(Builtin_GLSL_FishEye_PS))
        .SetHLSLPrecompiled(BuiltinHLSL_FishEye_PS, sizeof(BuiltinHLSL_FishEye_PS))
        .SetMetal(Builtin_Metal_FishEye_PS, std::strlen(Builtin_Metal_FishEye_PS), "FishEyePS");

    auto presentationParameters = graphicsDevice->GetPresentationParameters();

    pipelineState = assets.CreateBuilder<PipelineState>()
        .SetRenderTargetViewFormat(presentationParameters.BackBufferFormat)
        .SetDepthStencilViewFormat(presentationParameters.DepthStencilFormat)
        .SetVertexShader(vertexShader.Build())
        .SetPixelShader(pixelShader.Build())
        .SetInputLayout(inputLayout.CreateInputLayout())
        .SetBlendState(BlendDescription::CreateOpaque())
        .SetDepthStencilState(DepthStencilDescription::CreateNone())
        .SetConstantBufferBindSlot("ImageEffectConstants", 0)
        .SetConstantBufferBindSlot("FishEyeBlock", 1)
        .Build();

    constantBufferFishEye = std::make_shared<ConstantBuffer>(
        graphicsDevice, sizeof(FishEyeBlock), BufferUsage::Dynamic);

    SetStrength(0.04f);
}

void FishEyeEffect::SetStrength(float strength)
{
    FishEyeBlock block;
    block.Strength = MathHelper::Clamp(strength, -0.5f, 0.5f);

    POMDOG_ASSERT(constantBufferFishEye);
    constantBufferFishEye->SetValue(std::move(block));
}

void FishEyeEffect::Apply(GraphicsCommandList & commandList,
    std::shared_ptr<RenderTarget2D> const& source,
    std::shared_ptr<ConstantBuffer> const& constantBuffer)
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
