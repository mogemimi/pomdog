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
#include "../Utility/MakeUnique.hpp"

namespace Pomdog {
namespace Details {
namespace RenderSystem {
namespace GL4 {
//-----------------------------------------------------------------------
std::unique_ptr<NativeIndexBuffer>
GraphicsDeviceGL4::CreateIndexBuffer(void const* indices, std::size_t indexCount,
	IndexElementSize elementSize, BufferUsage bufferUsage)
{
	return MakeUnique<IndexBufferGL4>(indices, indexCount, elementSize, bufferUsage);
}
//-----------------------------------------------------------------------
std::unique_ptr<NativeVertexBuffer>
GraphicsDeviceGL4::CreateVertexBuffer(void const* vertices, std::size_t vertexCount,
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
}// namespace GL4
}// namespace RenderSystem
}// namespace Details
}// namespace Pomdog
