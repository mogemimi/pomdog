//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include <Pomdog/Graphics/VertexBuffer.hpp>
#include <utility>
#include <Pomdog/Utility/Assert.hpp>
#include <Pomdog/Graphics/BufferUsage.hpp>
#include <Pomdog/Graphics/GraphicsDevice.hpp>
#include "../RenderSystem/NativeGraphicsDevice.hpp"
#include "../RenderSystem/NativeVertexBuffer.hpp"

namespace Pomdog {
//-----------------------------------------------------------------------
VertexBuffer::VertexBuffer(std::shared_ptr<GraphicsDevice> const& graphicsDevice,
	Pomdog::VertexDeclaration const& vertexDeclarationIn,
	void const* vertices, std::uint32_t vertexCountIn, Pomdog::BufferUsage bufferUsageIn)
	: vertexDeclaration(vertexDeclarationIn)
	, nativeVertexBuffer(graphicsDevice->NativeGraphicsDevice()->CreateVertexBuffer(
		vertices, vertexCountIn, vertexDeclaration, bufferUsageIn))
	, vertexCount(vertexCountIn)
	, bufferUsage(bufferUsageIn)
{
	POMDOG_ASSERT(nativeVertexBuffer);
}
//-----------------------------------------------------------------------
VertexBuffer::VertexBuffer(std::shared_ptr<GraphicsDevice> const& graphicsDevice,
	Pomdog::VertexDeclaration && vertexDeclarationIn,
	void const* vertices, std::uint32_t vertexCountIn, Pomdog::BufferUsage bufferUsageIn)
	: vertexDeclaration(std::move(vertexDeclarationIn))
	, nativeVertexBuffer(graphicsDevice->NativeGraphicsDevice()->CreateVertexBuffer(
		vertices, vertexCountIn, vertexDeclaration, bufferUsageIn))
	, vertexCount(vertexCountIn)
	, bufferUsage(bufferUsageIn)
{
	POMDOG_ASSERT(nativeVertexBuffer);
}
//-----------------------------------------------------------------------
VertexBuffer::~VertexBuffer() = default;
//-----------------------------------------------------------------------
VertexDeclaration const& VertexBuffer::VertexDeclaration() const
{
	return vertexDeclaration;
}
//-----------------------------------------------------------------------
BufferUsage VertexBuffer::BufferUsage() const
{
	return bufferUsage;
}
//-----------------------------------------------------------------------
std::uint32_t VertexBuffer::VertexCount() const
{
	return vertexCount;
}
//-----------------------------------------------------------------------
void VertexBuffer::SetData(void const* source, std::uint32_t elementCountIn)
{
	POMDOG_ASSERT(source != nullptr);
	POMDOG_ASSERT(elementCountIn > 0);
	POMDOG_ASSERT(elementCountIn <= vertexCount);
	POMDOG_ASSERT(nativeVertexBuffer);
	POMDOG_ASSERT(bufferUsage != Pomdog::BufferUsage::Immutable);
	nativeVertexBuffer->SetData(source, elementCountIn, vertexDeclaration);
}
//-----------------------------------------------------------------------
Details::RenderSystem::NativeVertexBuffer* VertexBuffer::NativeVertexBuffer()
{
	POMDOG_ASSERT(nativeVertexBuffer);
	return nativeVertexBuffer.get();
}
//-----------------------------------------------------------------------
}// namespace Pomdog
