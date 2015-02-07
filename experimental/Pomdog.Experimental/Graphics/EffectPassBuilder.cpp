//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

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

public:
	explicit Impl(GraphicsDevice & graphicsDeviceIn)
		: graphicsDevice(graphicsDeviceIn)
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
		impl->description.VertexShader = HLSLCompiler::CreateVertexShader(
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
		impl->description.PixelShader = HLSLCompiler::CreatePixelShader(
			impl->graphicsDevice, shaderSource, byteLength, entryPoint);
	}

	return *this;
}
//-----------------------------------------------------------------------
EffectPassBuilder & EffectPassBuilder::InputElements(std::vector<VertexBufferBinding> const& inputElements)
{
	POMDOG_ASSERT(impl);
	impl->description.InputElements = inputElements;
	return *this;
}
//-----------------------------------------------------------------------
EffectPassBuilder & EffectPassBuilder::InputElements(std::vector<VertexBufferBinding> && inputElements)
{
	POMDOG_ASSERT(impl);
	impl->description.InputElements = std::move(inputElements);
	return *this;
}
//-----------------------------------------------------------------------
EffectPassBuilder & EffectPassBuilder::InputElements(VertexDeclaration const& vertexDeclaration)
{
	POMDOG_ASSERT(impl);
	impl->description.InputElements = {vertexDeclaration};
	return *this;
}
//-----------------------------------------------------------------------
EffectPassBuilder & EffectPassBuilder::InputElements(VertexDeclaration && vertexDeclaration)
{
	POMDOG_ASSERT(impl);
	impl->description.InputElements = {std::move(vertexDeclaration)};
	return *this;
}
//-----------------------------------------------------------------------
std::shared_ptr<EffectPass> EffectPassBuilder::Create()
{
	POMDOG_ASSERT(impl);

	auto effectPass = std::make_shared<EffectPass>(impl->graphicsDevice, impl->description);
	return std::move(effectPass);
}
//-----------------------------------------------------------------------
}// namespace Pomdog
