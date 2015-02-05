//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#include "GraphicsDeviceGL4.hpp"
#include "BlendStateGL4.hpp"
#include "ConstantBufferGL4.hpp"
#include "DepthStencilStateGL4.hpp"
#include "EffectPassGL4.hpp"
#include "EffectReflectionGL4.hpp"
#include "IndexBufferGL4.hpp"
#include "InputLayoutGL4.hpp"
#include "RasterizerStateGL4.hpp"
#include "SamplerStateGL4.hpp"
#include "Texture2DGL4.hpp"
#include "RenderTarget2DGL4.hpp"
#include "VertexBufferGL4.hpp"
#include "Pomdog/Graphics/ShaderLanguage.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Utility/Exception.hpp"

namespace Pomdog {
namespace Details {
namespace RenderSystem {
namespace GL4 {
//-----------------------------------------------------------------------
ShaderLanguage GraphicsDeviceGL4::GetSupportedLanguage() const
{
	return ShaderLanguage::GLSL;
}
//-----------------------------------------------------------------------
std::unique_ptr<NativeIndexBuffer>
GraphicsDeviceGL4::CreateIndexBuffer(std::uint32_t sizeInBytes, BufferUsage bufferUsage)
{
	return std::make_unique<IndexBufferGL4>(sizeInBytes, bufferUsage);
}
//-----------------------------------------------------------------------
std::unique_ptr<NativeIndexBuffer>
GraphicsDeviceGL4::CreateIndexBuffer(void const* indices,
	std::uint32_t sizeInBytes, BufferUsage bufferUsage)
{
	return std::make_unique<IndexBufferGL4>(indices, sizeInBytes, bufferUsage);
}
//-----------------------------------------------------------------------
std::unique_ptr<NativeVertexBuffer>
GraphicsDeviceGL4::CreateVertexBuffer(std::uint32_t sizeInBytes, BufferUsage bufferUsage)
{
	return std::make_unique<VertexBufferGL4>(sizeInBytes, bufferUsage);
}
//-----------------------------------------------------------------------
std::unique_ptr<NativeVertexBuffer>
GraphicsDeviceGL4::CreateVertexBuffer(void const* vertices,
	std::uint32_t sizeInBytes, BufferUsage bufferUsage)
{
	return std::make_unique<VertexBufferGL4>(vertices, sizeInBytes, bufferUsage);
}
//-----------------------------------------------------------------------
std::unique_ptr<NativeBlendState>
GraphicsDeviceGL4::CreateBlendState(BlendDescription const& description)
{
	return std::make_unique<BlendStateGL4>(description);
}
//-----------------------------------------------------------------------
std::unique_ptr<NativeDepthStencilState>
GraphicsDeviceGL4::CreateDepthStencilState(DepthStencilDescription const& description)
{
	return std::make_unique<DepthStencilStateGL4>(description);
}
//-----------------------------------------------------------------------
std::unique_ptr<NativeSamplerState>
GraphicsDeviceGL4::CreateSamplerState(SamplerDescription const& description)
{
	return std::make_unique<SamplerStateGL4>(description);
}
//-----------------------------------------------------------------------
std::unique_ptr<NativeRasterizerState>
GraphicsDeviceGL4::CreateRasterizerState(RasterizerDescription const& description)
{
	return std::make_unique<RasterizerStateGL4>(description);
}
//-----------------------------------------------------------------------
std::unique_ptr<NativeEffectPass>
GraphicsDeviceGL4::CreateEffectPass(EffectPassDescription const& description)
{
	return std::make_unique<EffectPassGL4>(description);
}
//-----------------------------------------------------------------------
std::unique_ptr<NativeConstantBuffer>
GraphicsDeviceGL4::CreateConstantBuffer(std::uint32_t byteConstants)
{
	return std::make_unique<ConstantBufferGL4>(byteConstants);
}
//-----------------------------------------------------------------------
std::unique_ptr<NativeEffectReflection>
GraphicsDeviceGL4::CreateEffectReflection(NativeEffectPass & nativeEffectPass)
{
	auto const effectPassGL4 = dynamic_cast<EffectPassGL4*>(&nativeEffectPass);
	POMDOG_ASSERT(effectPassGL4 != nullptr);

	return std::make_unique<EffectReflectionGL4>(effectPassGL4->GetShaderProgram());
}
//-----------------------------------------------------------------------
std::unique_ptr<NativeInputLayout>
GraphicsDeviceGL4::CreateInputLayout(NativeEffectPass & nativeEffectPass,
	std::vector<VertexBufferBinding> const& vertexBufferBindings)
{
	auto const effectPassGL4 = dynamic_cast<EffectPassGL4*>(&nativeEffectPass);
	POMDOG_ASSERT(effectPassGL4 != nullptr);

	return std::make_unique<InputLayoutGL4>(effectPassGL4->GetShaderProgram(), vertexBufferBindings);
}
//-----------------------------------------------------------------------
std::unique_ptr<NativeTexture2D>
GraphicsDeviceGL4::CreateTexture2D(std::int32_t width, std::int32_t height, std::uint32_t mipmapLevels,
	SurfaceFormat format)
{
	return std::make_unique<Texture2DGL4>(width, height, mipmapLevels, format);
}
//-----------------------------------------------------------------------
std::unique_ptr<NativeRenderTarget2D>
GraphicsDeviceGL4::CreateRenderTarget2D(std::int32_t width, std::int32_t height,
	std::uint32_t mipmapLevels, SurfaceFormat format, DepthFormat depthStencilFormat)
{
	return std::make_unique<RenderTarget2DGL4>(width, height, mipmapLevels, format, depthStencilFormat);
}
//-----------------------------------------------------------------------
}// namespace GL4
}// namespace RenderSystem
}// namespace Details
}// namespace Pomdog
