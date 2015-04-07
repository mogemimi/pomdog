// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#include "ChromaticAberration.hpp"
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
#include "Shaders/GLSL.Embedded/ScreenQuad_VS.inc.hpp"
#include "Shaders/GLSL.Embedded/ChromaticAberration_PS.inc.hpp"

struct BuiltinEffectChromaticAberrationTrait {
    static std::shared_ptr<EffectPass> Create(GraphicsDevice & graphicsDevice)
    {
        InputLayoutHelper inputLayout;
        inputLayout.Float3().Float2();

        auto effectPass = EffectPassBuilder(graphicsDevice)
            .VertexShaderGLSL(Builtin_GLSL_ScreenQuad_VS, std::strlen(Builtin_GLSL_ScreenQuad_VS))
            .PixelShaderGLSL(Builtin_GLSL_ChromaticAberration_PS, std::strlen(Builtin_GLSL_ChromaticAberration_PS))
            .InputLayout(inputLayout.CreateInputLayout())
            .BlendState(BlendDescription::CreateNonPremultiplied())
            .DepthStencilState(DepthStencilDescription::CreateNone())
            .Create();
        return std::move(effectPass);
    }
};

}// unnamed namespace
//-----------------------------------------------------------------------
ChromaticAberration::ChromaticAberration(std::shared_ptr<GraphicsDevice> const& graphicsDevice)
{
    samplerState = std::make_shared<SamplerState>(graphicsDevice,
        SamplerDescription::CreateLinearClamp());

    effectPass = effectPass = graphicsDevice->ShaderPool().Create<BuiltinEffectChromaticAberrationTrait>(*graphicsDevice);
    constantBuffers = std::make_shared<ConstantBufferBinding>(graphicsDevice, *effectPass);
}
//-----------------------------------------------------------------------
void ChromaticAberration::SetViewport(float width, float height)
{
    Vector2 renderTargetSize(width, height);
    constantBuffers->Find("Constants")->SetValue(renderTargetSize);
}
//-----------------------------------------------------------------------
void ChromaticAberration::SetTexture(std::shared_ptr<RenderTarget2D> const& textureIn)
{
    POMDOG_ASSERT(textureIn);
    texture = textureIn;
}
//-----------------------------------------------------------------------
void ChromaticAberration::Apply(GraphicsContext & graphicsContext)
{
    POMDOG_ASSERT(texture);
    graphicsContext.SetSamplerState(0, samplerState);
    graphicsContext.SetTexture(0, texture);
    graphicsContext.SetEffectPass(effectPass);
    graphicsContext.SetConstantBuffers(constantBuffers);
}
//-----------------------------------------------------------------------
}// namespace Pomdog
