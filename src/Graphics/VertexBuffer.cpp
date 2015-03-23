// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#include "Pomdog/Graphics/VertexBuffer.hpp"
#include "../RenderSystem/NativeGraphicsDevice.hpp"
#include "../RenderSystem/NativeVertexBuffer.hpp"
#include "Pomdog/Graphics/BufferUsage.hpp"
#include "Pomdog/Graphics/GraphicsDevice.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include <utility>

namespace Pomdog {
//-----------------------------------------------------------------------
VertexBuffer::VertexBuffer(GraphicsDevice & graphicsDevice,
	void const* vertices, std::size_t vertexCountIn,
	std::size_t strideBytesIn, Pomdog::BufferUsage bufferUsageIn)
	: nativeVertexBuffer(graphicsDevice.NativeGraphicsDevice()->CreateVertexBuffer(
		vertices, vertexCountIn * strideBytesIn, bufferUsageIn))
	, vertexCount(vertexCountIn)
	, strideBytes(strideBytesIn)
	, bufferUsage(bufferUsageIn)
{
	POMDOG_ASSERT(nativeVertexBuffer);
	POMDOG_ASSERT(vertices != nullptr);
	POMDOG_ASSERT(vertexCountIn > 0);
	POMDOG_ASSERT(strideBytesIn > 0);
}
//-----------------------------------------------------------------------
VertexBuffer::VertexBuffer(GraphicsDevice & graphicsDevice,
	std::size_t vertexCountIn, std::size_t strideBytesIn, Pomdog::BufferUsage bufferUsageIn)
	: nativeVertexBuffer(graphicsDevice.NativeGraphicsDevice()->CreateVertexBuffer(
		vertexCountIn * strideBytesIn, bufferUsageIn))
	, vertexCount(vertexCountIn)
	, strideBytes(strideBytesIn)
	, bufferUsage(bufferUsageIn)
{
	POMDOG_ASSERT(nativeVertexBuffer);
	POMDOG_ASSERT(bufferUsageIn != BufferUsage::Immutable);
	POMDOG_ASSERT(vertexCountIn > 0);
	POMDOG_ASSERT(strideBytesIn > 0);
}
//-----------------------------------------------------------------------
VertexBuffer::VertexBuffer(std::shared_ptr<GraphicsDevice> const& graphicsDevice,
	void const* vertices, std::size_t vertexCountIn,
	std::size_t strideBytesIn, Pomdog::BufferUsage bufferUsageIn)
	: VertexBuffer(*graphicsDevice, vertices, vertexCountIn, strideBytesIn, bufferUsageIn)
{
	POMDOG_ASSERT(nativeVertexBuffer);
}
//-----------------------------------------------------------------------
VertexBuffer::VertexBuffer(std::shared_ptr<GraphicsDevice> const& graphicsDevice,
	std::size_t vertexCountIn, std::size_t strideBytesIn, Pomdog::BufferUsage bufferUsageIn)
	: VertexBuffer(*graphicsDevice, vertexCountIn, strideBytesIn, bufferUsageIn)
{
	POMDOG_ASSERT(nativeVertexBuffer);
}
//-----------------------------------------------------------------------
VertexBuffer::~VertexBuffer() = default;
//-----------------------------------------------------------------------
std::size_t VertexBuffer::StrideBytes() const
{
	return strideBytes;
}
//-----------------------------------------------------------------------
BufferUsage VertexBuffer::BufferUsage() const
{
	return bufferUsage;
}
//-----------------------------------------------------------------------
std::size_t VertexBuffer::VertexCount() const
{
	return vertexCount;
}
//-----------------------------------------------------------------------
void VertexBuffer::SetData(void const* source, std::size_t elementCount)
{
	POMDOG_ASSERT(source != nullptr);
	POMDOG_ASSERT(elementCount > 0);
	POMDOG_ASSERT(elementCount <= vertexCount);
	POMDOG_ASSERT(nativeVertexBuffer);
	POMDOG_ASSERT(bufferUsage != Pomdog::BufferUsage::Immutable);
	nativeVertexBuffer->SetData(0, source, elementCount * strideBytes);
}
//-----------------------------------------------------------------------
void VertexBuffer::SetData(std::size_t offsetInBytes, void const* source,
	std::size_t elementCount, std::size_t strideBytesIn)
{
	POMDOG_ASSERT(source != nullptr);
	POMDOG_ASSERT(elementCount > 0);
	POMDOG_ASSERT(elementCount <= vertexCount);
	POMDOG_ASSERT(nativeVertexBuffer);
	POMDOG_ASSERT(bufferUsage != Pomdog::BufferUsage::Immutable);
	nativeVertexBuffer->SetData(offsetInBytes, source, elementCount * strideBytesIn);
}
//-----------------------------------------------------------------------
Detail::RenderSystem::NativeVertexBuffer* VertexBuffer::NativeVertexBuffer()
{
	POMDOG_ASSERT(nativeVertexBuffer);
	return nativeVertexBuffer.get();
}
//-----------------------------------------------------------------------
}// namespace Pomdog
