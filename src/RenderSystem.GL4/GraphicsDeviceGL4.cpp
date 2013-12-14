//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "GraphicsDeviceGL4.hpp"
#include <Pomdog/Utility/Exception.hpp>
#include "BlendStateGL4.hpp"
#include "DepthStencilStateGL4.hpp"
#include "IndexBufferGL4.hpp"
#include "RasterizerStateGL4.hpp"
#include "SamplerStateGL4.hpp"
#include "VertexBufferGL4.hpp"

namespace Pomdog {
namespace Details {
namespace RenderSystem {
namespace GL4 {
//-----------------------------------------------------------------------
std::unique_ptr<NativeIndexBuffer>
GraphicsDeviceGL4::CreateIndexBuffer(void const* indices, std::size_t indexCount,
	IndexElementSize elementSize, BufferUsage bufferUsage)
{
	return std::unique_ptr<IndexBufferGL4>(new IndexBufferGL4(
		indices, indexCount, elementSize, bufferUsage));
}
//-----------------------------------------------------------------------
std::unique_ptr<NativeVertexBuffer>
GraphicsDeviceGL4::CreateVertexBuffer(void const* vertices, std::size_t vertexCount,
	VertexDeclaration const& vertexDeclaration, BufferUsage bufferUsage)
{
	return std::unique_ptr<VertexBufferGL4>(new VertexBufferGL4(
		vertices, vertexCount, vertexDeclaration, bufferUsage));
}
//-----------------------------------------------------------------------
std::unique_ptr<NativeBlendState>
GraphicsDeviceGL4::CreateBlendState(BlendDescription const& description)
{
	static_assert(__cplusplus < 201401L, "TODO: support for std::make_unique");
	//return std::make_unique<BlendStateGL4>(description); // C++14
	return std::unique_ptr<BlendStateGL4>(new BlendStateGL4(description));
}
//-----------------------------------------------------------------------
std::unique_ptr<NativeDepthStencilState>
GraphicsDeviceGL4::CreateDepthStencilState(DepthStencilDescription const& description)
{
	static_assert(__cplusplus < 201401L, "TODO: support for std::make_unique");
	//return std::make_unique<DepthStencilStateGL4>(description); // C++14
	return std::unique_ptr<DepthStencilStateGL4>(new DepthStencilStateGL4(description));
}
//-----------------------------------------------------------------------
std::unique_ptr<NativeSamplerState>
GraphicsDeviceGL4::CreateSamplerState(SamplerDescription const& description)
{
	static_assert(__cplusplus < 201401L, "TODO: support for std::make_unique");
	//return std::make_unique<SamplerStateGL4>(description); // C++14
	return std::unique_ptr<SamplerStateGL4>(new SamplerStateGL4(description));
}
//-----------------------------------------------------------------------
std::unique_ptr<NativeRasterizerState>
GraphicsDeviceGL4::CreateRasterizerState(RasterizerDescription const& description)
{
	static_assert(__cplusplus < 201401L, "TODO: support for std::make_unique");
	//return std::make_unique<DepthStencilStateGL4>(description); // C++14
	return std::unique_ptr<RasterizerStateGL4>(new RasterizerStateGL4(description));
}
//-----------------------------------------------------------------------
	
}// namespace GL4
}// namespace RenderSystem
}// namespace Details
}// namespace Pomdog
