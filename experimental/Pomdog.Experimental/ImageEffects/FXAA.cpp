// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#include "FXAA.hpp"
#include "Pomdog/Content/AssetBuilders/PipelineStateBuilder.hpp"
#include "Pomdog/Content/AssetBuilders/ShaderBuilder.hpp"
#include "Pomdog/Graphics/BlendDescription.hpp"
#include "Pomdog/Graphics/ConstantBuffer.hpp"
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
#include "Shaders/GLSL.Embedded/FXAA_VS.inc.hpp"
#include "Shaders/GLSL.Embedded/FXAA_PS.inc.hpp"
#include "Shaders/HLSL.Embedded/FXAA_VS.inc.hpp"
#include "Shaders/HLSL.Embedded/FXAA_PS.inc.hpp"

} // unnamed namespace
//-----------------------------------------------------------------------
FXAA::FXAA(std::shared_ptr<GraphicsDevice> const& graphicsDevice,
    AssetManager & assets)
{
    samplerLinear = std::make_shared<SamplerState>(graphicsDevice,
        SamplerDescription::CreateLinearClamp());

    auto inputLayout = InputLayoutHelper{}
        .Float3().Float2();

    auto vertexShader = assets.CreateBuilder<Shader>()
        .SetPipelineStage(ShaderCompilers::ShaderPipelineStage::VertexShader)
        .SetGLSL(Builtin_GLSL_FXAA_VS, std::strlen(Builtin_GLSL_FXAA_VS))
        .SetHLSLPrecompiled(BuiltinHLSL_FXAA_VS, sizeof(BuiltinHLSL_FXAA_VS));

    auto pixelShader = assets.CreateBuilder<Shader>()
        .SetPipelineStage(ShaderCompilers::ShaderPipelineStage::PixelShader)
        .SetGLSL(Builtin_GLSL_FXAA_PS, std::strlen(Builtin_GLSL_FXAA_PS))
        .SetHLSLPrecompiled(BuiltinHLSL_FXAA_PS, sizeof(BuiltinHLSL_FXAA_PS));

    pipelineState = assets.CreateBuilder<PipelineState>()
        .SetVertexShader(vertexShader.Build())
        .SetPixelShader(pixelShader.Build())
        .SetInputLayout(inputLayout.CreateInputLayout())
        .SetBlendState(BlendDescription::CreateNonPremultiplied())
        .SetDepthStencilState(DepthStencilDescription::CreateNone())
        .SetConstantBufferBindSlot("ImageEffectConstants", 0)
        .Build();
}
//-----------------------------------------------------------------------
void FXAA::Apply(GraphicsCommandList & commandList,
    std::shared_ptr<RenderTarget2D> const& source,
    std::shared_ptr<ConstantBuffer> const& constantBuffer)
{
    POMDOG_ASSERT(source);
    POMDOG_ASSERT(constantBuffer);
    commandList.SetConstantBuffer(0, constantBuffer);
    commandList.SetSamplerState(0, samplerLinear);
    commandList.SetTexture(0, source);
    commandList.SetPipelineState(pipelineState);
}
//-----------------------------------------------------------------------
} // namespace Pomdog
