// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#include "SepiaToneEffect.hpp"
#include "Pomdog.Experimental/Graphics/EffectPassBuilder.hpp"
#include "Pomdog/Graphics/detail/BuiltinShaderPool.hpp"
#include "Pomdog/Graphics/BlendDescription.hpp"
#include "Pomdog/Graphics/ConstantBufferBinding.hpp"
#include "Pomdog/Graphics/DepthStencilDescription.hpp"
#include "Pomdog/Graphics/EffectPass.hpp"
#include "Pomdog/Graphics/EffectParameter.hpp"
#include "Pomdog/Graphics/GraphicsContext.hpp"
#include "Pomdog/Graphics/GraphicsDevice.hpp"
#include "Pomdog/Graphics/RenderTarget2D.hpp"
#include "Pomdog/Graphics/SamplerState.hpp"
#include "Pomdog/Graphics/VertexDeclaration.hpp"
#include "Pomdog/Math/Vector2.hpp"
#include "Pomdog/Utility/Assert.hpp"

namespace Pomdog {
namespace {

// Built-in shaders
#include "Shaders/GLSL.Embedded/ScreenQuad_VS.inc.hpp"
#include "Shaders/GLSL.Embedded/SepiaTone_PS.inc.hpp"

struct BuiltinEffectSepiaToneTrait {
    static std::shared_ptr<EffectPass> Create(GraphicsDevice & graphicsDevice)
    {
        auto effectPass = EffectPassBuilder(graphicsDevice)
            .VertexShaderGLSL(Builtin_GLSL_ScreenQuad_VS, std::strlen(Builtin_GLSL_ScreenQuad_VS))
            .PixelShaderGLSL(Builtin_GLSL_SepiaTone_PS, std::strlen(Builtin_GLSL_SepiaTone_PS))
            .InputElements({VertexElementFormat::Float3, VertexElementFormat::Float2})
            .BlendState(BlendDescription::CreateNonPremultiplied())
            .DepthStencilState(DepthStencilDescription::CreateNone())
            .Create();
        return std::move(effectPass);
    }
};

}// unnamed namespace
//-----------------------------------------------------------------------
SepiaToneEffect::SepiaToneEffect(std::shared_ptr<GraphicsDevice> const& graphicsDevice)
{
    samplerLinear = SamplerState::CreateLinearWrap(graphicsDevice);

    effectPass = graphicsDevice->ShaderPool().Create<BuiltinEffectSepiaToneTrait>(*graphicsDevice);
    constantBuffers = std::make_shared<ConstantBufferBinding>(graphicsDevice, *effectPass);
}
//-----------------------------------------------------------------------
void SepiaToneEffect::SetViewport(float width, float height)
{
    Vector2 renderTargetSize(width, height);
    constantBuffers->Find("Constants")->SetValue(renderTargetSize);
}
//-----------------------------------------------------------------------
void SepiaToneEffect::SetTexture(std::shared_ptr<RenderTarget2D> const& textureIn)
{
    POMDOG_ASSERT(textureIn);
    texture = textureIn;
}
//-----------------------------------------------------------------------
void SepiaToneEffect::Apply(GraphicsContext & graphicsContext)
{
    POMDOG_ASSERT(texture);

    graphicsContext.SetSamplerState(0, samplerLinear);
    graphicsContext.SetTexture(0, texture);
    graphicsContext.SetEffectPass(effectPass);
    graphicsContext.SetConstantBuffers(constantBuffers);
}
//-----------------------------------------------------------------------
}// namespace Pomdog
