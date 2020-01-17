// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#include "Pomdog/Experimental/ImageEffects/RetroCrtEffect.hpp"
#include "Pomdog/Content/AssetBuilders/PipelineStateBuilder.hpp"
#include "Pomdog/Content/AssetBuilders/ShaderBuilder.hpp"
#include "Pomdog/Graphics/BlendDescription.hpp"
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
#include "Pomdog/Utility/Assert.hpp"
#include <cstring>

namespace Pomdog {
namespace {

// Built-in shaders
#include "Shaders/GLSL.Embedded/RetroCrtEffect_PS.inc.hpp"
#include "Shaders/GLSL.Embedded/ScreenQuad_VS.inc.hpp"
#include "Shaders/HLSL.Embedded/RetroCrtEffect_PS.inc.hpp"
#include "Shaders/HLSL.Embedded/ScreenQuad_VS.inc.hpp"
#include "Shaders/Metal.Embedded/RetroCrtEffect_PS.inc.hpp"
#include "Shaders/Metal.Embedded/ScreenQuad_VS.inc.hpp"

} // unnamed namespace

RetroCrtEffect::RetroCrtEffect(
    const std::shared_ptr<GraphicsDevice>& graphicsDevice,
    AssetManager& assets)
{
    samplerState = std::make_shared<SamplerState>(
        graphicsDevice, SamplerDescription::CreateLinearClamp());

    auto inputLayout = InputLayoutHelper{}
        .Float3().Float2();

    auto vertexShader = assets.CreateBuilder<Shader>(ShaderPipelineStage::VertexShader)
        .SetGLSL(Builtin_GLSL_ScreenQuad_VS, std::strlen(Builtin_GLSL_ScreenQuad_VS))
        .SetHLSLPrecompiled(BuiltinHLSL_ScreenQuad_VS, sizeof(BuiltinHLSL_ScreenQuad_VS))
        .SetMetal(Builtin_Metal_ScreenQuad_VS, std::strlen(Builtin_Metal_ScreenQuad_VS), "ScreenQuadVS");

    auto pixelShader = assets.CreateBuilder<Shader>(ShaderPipelineStage::PixelShader)
        .SetGLSL(Builtin_GLSL_RetroCrtEffect_PS, std::strlen(Builtin_GLSL_RetroCrtEffect_PS))
        .SetHLSLPrecompiled(BuiltinHLSL_RetroCrtEffect_PS, sizeof(BuiltinHLSL_RetroCrtEffect_PS))
        .SetMetal(Builtin_Metal_RetroCrtEffect_PS, std::strlen(Builtin_Metal_RetroCrtEffect_PS), "RetroCrtEffectPS");

    auto presentationParameters = graphicsDevice->GetPresentationParameters();

    pipelineState = assets.CreateBuilder<PipelineState>()
        .SetRenderTargetViewFormat(presentationParameters.BackBufferFormat)
        .SetDepthStencilViewFormat(presentationParameters.DepthStencilFormat)
        .SetVertexShader(vertexShader.Build())
        .SetPixelShader(pixelShader.Build())
        .SetInputLayout(inputLayout.CreateInputLayout())
        .SetPrimitiveTopology(PrimitiveTopology::TriangleList)
        .SetBlendState(BlendDescription::CreateOpaque())
        .SetDepthStencilState(DepthStencilDescription::CreateNone())
        .SetConstantBufferBindSlot("ImageEffectConstants", 0)
        .Build();
}

void RetroCrtEffect::Apply(GraphicsCommandList& commandList,
    std::shared_ptr<RenderTarget2D> const& source,
    std::shared_ptr<ConstantBuffer> const& constantBuffer)
{
    POMDOG_ASSERT(source);
    POMDOG_ASSERT(constantBuffer);
    commandList.SetConstantBuffer(0, constantBuffer);
    commandList.SetSamplerState(0, samplerState);
    commandList.SetTexture(0, source);
    commandList.SetPipelineState(pipelineState);
}

} // namespace Pomdog
