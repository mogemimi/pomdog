// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#include "EffectPassBuilder.hpp"
#include "Pomdog/Graphics/ShaderCompilers/GLSLCompiler.hpp"
#include "Pomdog/Graphics/ShaderCompilers/HLSLCompiler.hpp"
#include "Pomdog/Graphics/EffectPassDescription.hpp"
#include "Pomdog/Graphics/Shader.hpp"
#include "Pomdog/Graphics/ShaderLanguage.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Utility/Optional.hpp"
#include <utility>

namespace Pomdog {
//-----------------------------------------------------------------------
using ShaderCompilers::GLSLCompiler;
using ShaderCompilers::HLSLCompiler;
//-----------------------------------------------------------------------
class EffectPassBuilder::Impl final {
public:
    GraphicsDevice & graphicsDevice;
    EffectPassDescription description;
    bool hasBlendState;
    bool hasRasterizerState;
    bool hasDepthStencilState;

public:
    explicit Impl(GraphicsDevice & graphicsDeviceIn)
        : graphicsDevice(graphicsDeviceIn)
        , hasBlendState(false)
        , hasRasterizerState(false)
        , hasDepthStencilState(false)
    {}
};
//-----------------------------------------------------------------------
EffectPassBuilder::EffectPassBuilder(GraphicsDevice & graphicsDevice)
    : impl(std::make_unique<Impl>(graphicsDevice))
{}
//-----------------------------------------------------------------------
EffectPassBuilder::EffectPassBuilder(std::shared_ptr<GraphicsDevice> const& graphicsDevice)
    : EffectPassBuilder(*graphicsDevice)
{}
//-----------------------------------------------------------------------
EffectPassBuilder::EffectPassBuilder(EffectPassBuilder &&) = default;
EffectPassBuilder & EffectPassBuilder::operator=(EffectPassBuilder &&) = default;
EffectPassBuilder::~EffectPassBuilder() = default;
//-----------------------------------------------------------------------
EffectPassBuilder & EffectPassBuilder::VertexShaderGLSL(void const* shaderSource, std::size_t byteLength)
{
    POMDOG_ASSERT(shaderSource != nullptr);
    POMDOG_ASSERT(byteLength > 0);
    POMDOG_ASSERT(impl);

    if (impl->graphicsDevice.GetSupportedLanguage() == ShaderLanguage::GLSL) {
        impl->description.VertexShader = GLSLCompiler::CreateVertexShader(
            impl->graphicsDevice, shaderSource, byteLength);
    }

    return *this;
}
//-----------------------------------------------------------------------
EffectPassBuilder & EffectPassBuilder::PixelShaderGLSL(void const* shaderSource, std::size_t byteLength)
{
    POMDOG_ASSERT(shaderSource != nullptr);
    POMDOG_ASSERT(byteLength > 0);
    POMDOG_ASSERT(impl);

    if (impl->graphicsDevice.GetSupportedLanguage() == ShaderLanguage::GLSL) {
        impl->description.PixelShader = GLSLCompiler::CreatePixelShader(
            impl->graphicsDevice, shaderSource, byteLength);
    }

    return *this;
}
//-----------------------------------------------------------------------
EffectPassBuilder & EffectPassBuilder::VertexShaderHLSL(void const* shaderSource, std::size_t byteLength, std::string const& entryPoint)
{
    POMDOG_ASSERT(shaderSource != nullptr);
    POMDOG_ASSERT(byteLength > 0);
    POMDOG_ASSERT(impl);

    if (impl->graphicsDevice.GetSupportedLanguage() == ShaderLanguage::HLSL) {
        impl->description.VertexShader = HLSLCompiler::CreateVertexShaderFromSource(
            impl->graphicsDevice, shaderSource, byteLength, entryPoint);
    }

    return *this;
}
//-----------------------------------------------------------------------
EffectPassBuilder & EffectPassBuilder::PixelShaderHLSL(void const* shaderSource, std::size_t byteLength, std::string const& entryPoint)
{
    POMDOG_ASSERT(shaderSource != nullptr);
    POMDOG_ASSERT(byteLength > 0);
    POMDOG_ASSERT(impl);

    if (impl->graphicsDevice.GetSupportedLanguage() == ShaderLanguage::HLSL) {
        impl->description.PixelShader = HLSLCompiler::CreatePixelShaderFromSource(
            impl->graphicsDevice, shaderSource, byteLength, entryPoint);
    }

    return *this;
}
//-----------------------------------------------------------------------
EffectPassBuilder & EffectPassBuilder::VertexShaderHLSLPrecompiled(void const* shaderSource, std::size_t byteLength)
{
    POMDOG_ASSERT(shaderSource != nullptr);
    POMDOG_ASSERT(byteLength > 0);
    POMDOG_ASSERT(impl);

    if (impl->graphicsDevice.GetSupportedLanguage() == ShaderLanguage::HLSL) {
        impl->description.VertexShader = HLSLCompiler::CreateVertexShader(
            impl->graphicsDevice, shaderSource, byteLength);
    }

    return *this;
}
//-----------------------------------------------------------------------
EffectPassBuilder & EffectPassBuilder::PixelShaderHLSLPrecompiled(void const* shaderSource, std::size_t byteLength)
{
    POMDOG_ASSERT(shaderSource != nullptr);
    POMDOG_ASSERT(byteLength > 0);
    POMDOG_ASSERT(impl);

    if (impl->graphicsDevice.GetSupportedLanguage() == ShaderLanguage::HLSL) {
        impl->description.PixelShader = HLSLCompiler::CreatePixelShader(
            impl->graphicsDevice, shaderSource, byteLength);
    }

    return *this;
}
//-----------------------------------------------------------------------
EffectPassBuilder & EffectPassBuilder::InputLayout(InputLayoutDescription const& inputLayout)
{
    POMDOG_ASSERT(impl);
    impl->description.InputLayout = inputLayout;
    return *this;
}
//-----------------------------------------------------------------------
EffectPassBuilder & EffectPassBuilder::InputLayout(InputLayoutDescription && inputLayout)
{
    POMDOG_ASSERT(impl);
    impl->description.InputLayout = std::move(inputLayout);
    return *this;
}
//-----------------------------------------------------------------------
EffectPassBuilder & EffectPassBuilder::BlendState(BlendDescription const& blendState)
{
    POMDOG_ASSERT(impl);
    impl->description.BlendState = blendState;
    impl->hasBlendState = true;
    return *this;
}
//-----------------------------------------------------------------------
EffectPassBuilder & EffectPassBuilder::RasterizerState(RasterizerDescription const& rasterizerState)
{
    POMDOG_ASSERT(impl);
    impl->description.RasterizerState = rasterizerState;
    impl->hasRasterizerState = true;
    return *this;
}
//-----------------------------------------------------------------------
EffectPassBuilder & EffectPassBuilder::DepthStencilState(DepthStencilDescription const& depthStencilState)
{
    POMDOG_ASSERT(impl);
    impl->description.DepthStencilState = depthStencilState;
    impl->hasDepthStencilState = true;
    return *this;
}
//-----------------------------------------------------------------------
std::shared_ptr<EffectPass> EffectPassBuilder::Create()
{
    POMDOG_ASSERT(impl);
    POMDOG_ASSERT(!impl->description.InputLayout.InputElements.empty());

    if (!impl->hasBlendState) {
        impl->description.BlendState = BlendDescription::CreateDefault();
        impl->hasBlendState = true;
    }

    if (!impl->hasRasterizerState) {
        impl->description.RasterizerState = RasterizerDescription::CreateDefault();
        impl->hasRasterizerState = true;
    }

    if (!impl->hasDepthStencilState) {
        impl->description.DepthStencilState = DepthStencilDescription::CreateDefault();
        impl->hasDepthStencilState = true;
    }

    auto effectPass = std::make_shared<EffectPass>(impl->graphicsDevice, impl->description);
    return std::move(effectPass);
}
//-----------------------------------------------------------------------
}// namespace Pomdog
