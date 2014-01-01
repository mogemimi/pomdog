//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "GraphicsDeviceGL4.hpp"
#include <Pomdog/Utility/Exception.hpp>
#include "BlendStateGL4.hpp"
#include "DepthStencilStateGL4.hpp"
#include "EffectPassGL4.hpp"
#include "IndexBufferGL4.hpp"
#include "InputLayoutGL4.hpp"
#include "RasterizerStateGL4.hpp"
#include "SamplerStateGL4.hpp"
#include "VertexBufferGL4.hpp"
#include "../Utility/MakeUnique.hpp"

namespace Pomdog {
namespace Details {
namespace RenderSystem {
namespace GL4 {
//-----------------------------------------------------------------------
std::unique_ptr<NativeIndexBuffer>
GraphicsDeviceGL4::CreateIndexBuffer(void const* indices, std::uint32_t indexCount,
	IndexElementSize elementSize, BufferUsage bufferUsage)
{
	return MakeUnique<IndexBufferGL4>(indices, indexCount, elementSize, bufferUsage);
}
//-----------------------------------------------------------------------
std::unique_ptr<NativeVertexBuffer>
GraphicsDeviceGL4::CreateVertexBuffer(void const* vertices, std::uint32_t vertexCount,
	VertexDeclaration const& vertexDeclaration, BufferUsage bufferUsage)
{
	return MakeUnique<VertexBufferGL4>(vertices, vertexCount, vertexDeclaration, bufferUsage);
}
//-----------------------------------------------------------------------
std::unique_ptr<NativeBlendState>
GraphicsDeviceGL4::CreateBlendState(BlendDescription const& description)
{
	return MakeUnique<BlendStateGL4>(description);
}
//-----------------------------------------------------------------------
std::unique_ptr<NativeDepthStencilState>
GraphicsDeviceGL4::CreateDepthStencilState(DepthStencilDescription const& description)
{
	return MakeUnique<DepthStencilStateGL4>(description);
}
//-----------------------------------------------------------------------
std::unique_ptr<NativeSamplerState>
GraphicsDeviceGL4::CreateSamplerState(SamplerDescription const& description)
{
	return MakeUnique<SamplerStateGL4>(description);
}
//-----------------------------------------------------------------------
std::unique_ptr<NativeRasterizerState>
GraphicsDeviceGL4::CreateRasterizerState(RasterizerDescription const& description)
{
	return MakeUnique<RasterizerStateGL4>(description);
}
//-----------------------------------------------------------------------
std::unique_ptr<NativeEffectPass>
GraphicsDeviceGL4::CreateEffectPass(ShaderBytecode const& vertexShaderBytecode, ShaderBytecode const& pixelShaderBytecode)
{
	return MakeUnique<EffectPassGL4>(vertexShaderBytecode, pixelShaderBytecode);
}
//-----------------------------------------------------------------------
std::unique_ptr<NativeInputLayout>
GraphicsDeviceGL4::CreateInputLayout(NativeEffectPass* nativeEffectPass)
{
	auto const effectPassGL4 = dynamic_cast<EffectPassGL4*>(nativeEffectPass);
	POMDOG_ASSERT(effectPassGL4 != nullptr);
	
	if (!effectPassGL4) {
		return std::unique_ptr<NativeInputLayout>();
	}
	return MakeUnique<InputLayoutGL4>(effectPassGL4);
}
//-----------------------------------------------------------------------
std::unique_ptr<NativeInputLayout>
GraphicsDeviceGL4::CreateInputLayout(NativeEffectPass* nativeEffectPass,
	std::vector<VertexBufferBinding> const& vertexBufferBindings)
{
	auto const effectPassGL4 = dynamic_cast<EffectPassGL4*>(nativeEffectPass);
	POMDOG_ASSERT(effectPassGL4 != nullptr);
	
	if (!effectPassGL4) {
		return std::unique_ptr<NativeInputLayout>();
	}
	return MakeUnique<InputLayoutGL4>(effectPassGL4, vertexBufferBindings);
}
//-----------------------------------------------------------------------
}// namespace GL4
}// namespace RenderSystem
}// namespace Details
}// namespace Pomdog
