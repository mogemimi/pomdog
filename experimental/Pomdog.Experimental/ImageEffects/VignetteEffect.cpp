// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#include "VignetteEffect.hpp"
#include "Pomdog/Content/AssetBuilders/PipelineStateBuilder.hpp"
#include "Pomdog/Content/AssetBuilders/ShaderBuilder.hpp"
#include "Pomdog/Graphics/BlendDescription.hpp"
#include "Pomdog/Graphics/ConstantBuffer.hpp"
#include "Pomdog/Graphics/ConstantBufferBinding.hpp"
#include "Pomdog/Graphics/DepthStencilDescription.hpp"
#include "Pomdog/Graphics/GraphicsCommandList.hpp"
#include "Pomdog/Graphics/GraphicsDevice.hpp"
#include "Pomdog/Graphics/InputLayoutHelper.hpp"
#include "Pomdog/Graphics/PipelineState.hpp"
#include "Pomdog/Graphics/RenderTarget2D.hpp"
#include "Pomdog/Graphics/SamplerState.hpp"
#include "Pomdog/Graphics/Shader.hpp"
#include "Pomdog/Utility/Assert.hpp"

namespace Pomdog {
namespace {

// Built-in shaders
#include "Shaders/GLSL.Embedded/ScreenQuad_VS.inc.hpp"
#include "Shaders/GLSL.Embedded/Vignette_PS.inc.hpp"
#include "Shaders/HLSL.Embedded/ScreenQuad_VS.inc.hpp"
#include "Shaders/HLSL.Embedded/Vignette_PS.inc.hpp"

} // unnamed namespace
//-----------------------------------------------------------------------
VignetteEffect::VignetteEffect(
    std::shared_ptr<GraphicsDevice> const& graphicsDevice,
    AssetManager & assets)
{
    samplerLinear = std::make_shared<SamplerState>(graphicsDevice,
        SamplerDescription::CreateLinearWrap());

    auto inputLayout = InputLayoutHelper{}
        .Float3().Float2();

    auto vertexShader = assets.CreateBuilder<Shader>()
        .SetPipelineStage(ShaderCompilers::ShaderPipelineStage::VertexShader)
        .SetGLSL(Builtin_GLSL_ScreenQuad_VS, std::strlen(Builtin_GLSL_ScreenQuad_VS))
        .SetHLSLPrecompiled(BuiltinHLSL_ScreenQuad_VS, sizeof(BuiltinHLSL_ScreenQuad_VS));

    auto pixelShader = assets.CreateBuilder<Shader>()
        .SetPipelineStage(ShaderCompilers::ShaderPipelineStage::PixelShader)
        .SetGLSL(Builtin_GLSL_Vignette_PS, std::strlen(Builtin_GLSL_Vignette_PS))
        .SetHLSLPrecompiled(BuiltinHLSL_Vignette_PS, sizeof(BuiltinHLSL_Vignette_PS));

    auto builder = assets.CreateBuilder<PipelineState>();
    pipelineState = builder
        .SetVertexShader(vertexShader.Build())
        .SetPixelShader(pixelShader.Build())
        .SetInputLayout(inputLayout.CreateInputLayout())
        .SetBlendState(BlendDescription::CreateNonPremultiplied())
        .SetDepthStencilState(DepthStencilDescription::CreateNone())
        .Build();

    constantBuffers = builder.CreateConstantBuffers(pipelineState);

    SetIntensity(0.5f);
}
//-----------------------------------------------------------------------
void VignetteEffect::SetIntensity(float intensity)
{
    struct VignetteBlock {
        float Intensity;
    };

    VignetteBlock block;
    block.Intensity = intensity;

    auto constantBuffer = constantBuffers->FindConstantBuffer("VignetteBlock");
    constantBuffer->SetValue(std::move(block));
}
//-----------------------------------------------------------------------
void VignetteEffect::BindConstantBuffer(std::shared_ptr<ConstantBuffer> const& /*constantBuffer*/)
{
    //POMDOG_ASSERT(constantBuffer);
    //constantBuffers->SetConstantBuffer("ImageEffectConstants", constantBuffer);
}
//-----------------------------------------------------------------------
void VignetteEffect::Apply(GraphicsCommandList & commandList,
    std::shared_ptr<RenderTarget2D> const& source)
{
    POMDOG_ASSERT(source);
    commandList.SetSamplerState(0, samplerLinear);
    commandList.SetTexture(0, source);
    commandList.SetPipelineState(pipelineState);
    commandList.SetConstantBuffers(constantBuffers);
}
//-----------------------------------------------------------------------
} // namespace Pomdog
