// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#include "FXAA.hpp"
#include "Pomdog.Experimental/Graphics/EffectPassBuilder.hpp"
#include "Pomdog/Graphics/detail/BuiltinShaderPool.hpp"
#include "Pomdog/Graphics/BlendDescription.hpp"
#include "Pomdog/Graphics/ConstantBuffer.hpp"
#include "Pomdog/Graphics/ConstantBufferBinding.hpp"
#include "Pomdog/Graphics/DepthStencilDescription.hpp"
#include "Pomdog/Graphics/EffectPass.hpp"
#include "Pomdog/Graphics/GraphicsContext.hpp"
#include "Pomdog/Graphics/GraphicsDevice.hpp"
#include "Pomdog/Graphics/InputLayoutHelper.hpp"
#include "Pomdog/Graphics/RenderTarget2D.hpp"
#include "Pomdog/Graphics/SamplerState.hpp"
#include "Pomdog/Math/Vector2.hpp"
#include "Pomdog/Utility/Assert.hpp"

namespace Pomdog {
namespace {

// Built-in shaders
#include "Shaders/GLSL.Embedded/FXAA_VS.inc.hpp"
#include "Shaders/GLSL.Embedded/FXAA_PS.inc.hpp"
#include "Shaders/HLSL.Embedded/FXAA_VS.inc.hpp"
#include "Shaders/HLSL.Embedded/FXAA_PS.inc.hpp"

struct BuiltinEffectFxaaTrait {
    static std::shared_ptr<EffectPass> Create(GraphicsDevice & graphicsDevice)
    {
        InputLayoutHelper inputLayout;
        inputLayout.Float3().Float2();

        auto effectPass = EffectPassBuilder(graphicsDevice)
            .VertexShaderGLSL(Builtin_GLSL_FXAA_VS, std::strlen(Builtin_GLSL_FXAA_VS))
            .PixelShaderGLSL(Builtin_GLSL_FXAA_PS, std::strlen(Builtin_GLSL_FXAA_PS))
            .VertexShaderHLSLPrecompiled(BuiltinHLSL_FXAA_VS, sizeof(BuiltinHLSL_FXAA_VS))
            .PixelShaderHLSLPrecompiled(BuiltinHLSL_FXAA_PS, sizeof(BuiltinHLSL_FXAA_PS))
            .InputLayout(inputLayout.CreateInputLayout())
            .BlendState(BlendDescription::CreateNonPremultiplied())
            .DepthStencilState(DepthStencilDescription::CreateNone())
            .Create();
        return std::move(effectPass);
    }
};

}// unnamed namespace
//-----------------------------------------------------------------------
FXAA::FXAA(std::shared_ptr<GraphicsDevice> const& graphicsDevice)
{
    samplerLinear = std::make_shared<SamplerState>(graphicsDevice,
        SamplerDescription::CreateLinearClamp());

    effectPass = graphicsDevice->ShaderPool().Create<BuiltinEffectFxaaTrait>(*graphicsDevice);
    constantBuffers = std::make_shared<ConstantBufferBinding>(graphicsDevice, *effectPass);
}
//-----------------------------------------------------------------------
void FXAA::SetViewport(float width, float height)
{
    Vector2 rcpFrame = Vector2(1.0f, 1.0f) / Vector2(width, height);
    constantBuffers->Find("FxaaPassBuffer")->SetValue(rcpFrame);
}
//-----------------------------------------------------------------------
void FXAA::SetTexture(std::shared_ptr<RenderTarget2D> const& textureIn)
{
    POMDOG_ASSERT(textureIn);
    texture = textureIn;
}
//-----------------------------------------------------------------------
void FXAA::Apply(GraphicsContext & graphicsContext)
{
    POMDOG_ASSERT(texture);

    graphicsContext.SetSamplerState(0, samplerLinear);
    graphicsContext.SetTexture(0, texture);
    graphicsContext.SetEffectPass(effectPass);
    graphicsContext.SetConstantBuffers(constantBuffers);
}
//-----------------------------------------------------------------------
}// namespace Pomdog
