//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include <Pomdog/Graphics/ImmutableVertexBuffer.hpp>
#include <utility>
#include <Pomdog/Utility/Assert.hpp>
#include <Pomdog/Graphics/BufferUsage.hpp>
#include <Pomdog/Graphics/GraphicsDevice.hpp>
#include "../RenderSystem/NativeGraphicsDevice.hpp"
#include "../RenderSystem/NativeVertexBuffer.hpp"

namespace Pomdog {
//-----------------------------------------------------------------------
ImmutableVertexBuffer::ImmutableVertexBuffer(std::shared_ptr<GraphicsDevice> const& graphicsDevice,
	Pomdog::VertexDeclaration const& newVertexDeclaration, void const* vertices, std::uint32_t newVertexCount)
	: vertexDeclaration(newVertexDeclaration)
	, nativeVertexBuffer(graphicsDevice->NativeGraphicsDevice()->CreateVertexBuffer(
		vertices, newVertexCount, vertexDeclaration, BufferUsage::Immutable))
	, vertexCount(newVertexCount)
{
	POMDOG_ASSERT(nativeVertexBuffer);
}
//-----------------------------------------------------------------------
ImmutableVertexBuffer::ImmutableVertexBuffer(std::shared_ptr<GraphicsDevice> const& graphicsDevice,
	Pomdog::VertexDeclaration && newVertexDeclaration, void const* vertices, std::uint32_t newVertexCount)
	: vertexDeclaration(std::move(newVertexDeclaration))
	, nativeVertexBuffer(graphicsDevice->NativeGraphicsDevice()->CreateVertexBuffer(
		vertices, newVertexCount, vertexDeclaration, BufferUsage::Immutable))
	, vertexCount(newVertexCount)
{
	POMDOG_ASSERT(nativeVertexBuffer);
}
//-----------------------------------------------------------------------
ImmutableVertexBuffer::~ImmutableVertexBuffer() = default;
//-----------------------------------------------------------------------
VertexDeclaration const& ImmutableVertexBuffer::VertexDeclaration() const
{
	return vertexDeclaration;
}
//-----------------------------------------------------------------------
BufferUsage ImmutableVertexBuffer::BufferUsage() const
{
	return BufferUsage::Immutable;
}
//-----------------------------------------------------------------------
std::uint32_t ImmutableVertexBuffer::VertexCount() const
{
	return vertexCount;
}
//-----------------------------------------------------------------------
Details::RenderSystem::NativeVertexBuffer* ImmutableVertexBuffer::NativeVertexBuffer()
{
	POMDOG_ASSERT(nativeVertexBuffer);
	return nativeVertexBuffer.get();
}
//-----------------------------------------------------------------------
}// namespace Pomdog
