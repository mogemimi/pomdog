//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include <Pomdog/Graphics/ImmutableVertexBuffer.hpp>
#include <utility>
#include <Pomdog/Utility/Assert.hpp>
#include <Pomdog/Graphics/GraphicsDevice.hpp>
#include "../RenderSystem/NativeGraphicsDevice.hpp"
#include "../RenderSystem/NativeVertexBuffer.hpp"
#include "BufferUsage.hpp"

namespace Pomdog {
//-----------------------------------------------------------------------
ImmutableVertexBuffer::ImmutableVertexBuffer(std::shared_ptr<GraphicsDevice> const& graphicsDevice,
	VertexDeclaration const& newVertexDeclaration, void const* vertices, std::size_t newVertexCount)
	: vertexDeclaration(newVertexDeclaration)
	, nativeVertexBuffer(graphicsDevice->GetNativeGraphicsDevice()->CreateVertexBuffer(
		vertices, newVertexCount,
		vertexDeclaration,
		BufferUsage::Immutable))
	, vertexCount(newVertexCount)
{
}
//-----------------------------------------------------------------------
ImmutableVertexBuffer::ImmutableVertexBuffer(std::shared_ptr<GraphicsDevice> const& graphicsDevice,
	VertexDeclaration && newVertexDeclaration, void const* vertices, std::size_t newVertexCount)
	: vertexDeclaration(std::move(newVertexDeclaration))
	, nativeVertexBuffer(graphicsDevice->GetNativeGraphicsDevice()->CreateVertexBuffer(
		vertices, newVertexCount,
		vertexDeclaration,
		BufferUsage::Immutable))
	, vertexCount(newVertexCount)
{
	POMDOG_ASSERT(nativeVertexBuffer);
}
//-----------------------------------------------------------------------
ImmutableVertexBuffer::~ImmutableVertexBuffer()
{
}
//-----------------------------------------------------------------------
VertexDeclaration const& ImmutableVertexBuffer::GetVertexDeclaration() const
{
	return vertexDeclaration;
}
//-----------------------------------------------------------------------
BufferUsage ImmutableVertexBuffer::GetBufferUsage() const
{
	return BufferUsage::Immutable;
}
//-----------------------------------------------------------------------
std::size_t ImmutableVertexBuffer::GetVertexCount() const
{
	return vertexCount;
}
//-----------------------------------------------------------------------
Details::RenderSystem::NativeVertexBuffer* ImmutableVertexBuffer::GetNativeVertexBuffer()
{
	return nativeVertexBuffer.get();
}
//-----------------------------------------------------------------------
}// namespace Pomdog
