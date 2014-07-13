//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include <Pomdog/Graphics/IndexBuffer.hpp>
#include <Pomdog/Utility/Assert.hpp>
#include <Pomdog/Graphics/GraphicsDevice.hpp>
#include "../RenderSystem/NativeGraphicsDevice.hpp"
#include "../RenderSystem/NativeIndexBuffer.hpp"

namespace Pomdog {
namespace {

static std::uint32_t ToIndexElementOffsetBytes(IndexElementSize elementSize)
{
	switch (elementSize) {
	case IndexElementSize::ThirtyTwoBits: return 4;
	case IndexElementSize::SixteenBits: return 2;
	};
#ifdef _MSC_VER
	return 4;// FUS RO DAH!!!
#endif
}

}// unnamed namespace
//-----------------------------------------------------------------------
IndexBuffer::IndexBuffer(std::shared_ptr<GraphicsDevice> const& graphicsDevice,
	IndexElementSize elementSizeIn, void const* indices, std::uint32_t indexCountIn,
	Pomdog::BufferUsage bufferUsageIn)
	: nativeIndexBuffer(graphicsDevice->NativeGraphicsDevice()->CreateIndexBuffer(
		indices, ToIndexElementOffsetBytes(elementSizeIn) * indexCountIn, bufferUsageIn))
	, indexCount(indexCountIn)
	, elementSize(elementSizeIn)
	, bufferUsage(bufferUsageIn)
{
	POMDOG_ASSERT(nativeIndexBuffer);
}
//-----------------------------------------------------------------------
IndexBuffer::IndexBuffer(std::shared_ptr<GraphicsDevice> const& graphicsDevice,
	IndexElementSize elementSizeIn, std::uint32_t indexCountIn,
	Pomdog::BufferUsage bufferUsageIn)
	: nativeIndexBuffer(graphicsDevice->NativeGraphicsDevice()->CreateIndexBuffer(
		ToIndexElementOffsetBytes(elementSizeIn) * indexCountIn, bufferUsageIn))
	, indexCount(indexCountIn)
	, elementSize(elementSizeIn)
	, bufferUsage(bufferUsageIn)
{
	POMDOG_ASSERT(nativeIndexBuffer);
}
//-----------------------------------------------------------------------
IndexBuffer::~IndexBuffer() = default;
//-----------------------------------------------------------------------
std::uint32_t IndexBuffer::IndexCount() const
{
	return indexCount;
}
//-----------------------------------------------------------------------
IndexElementSize IndexBuffer::ElementSize() const
{
	return elementSize;
}
//-----------------------------------------------------------------------
BufferUsage IndexBuffer::BufferUsage() const
{
	return bufferUsage;
}
//-----------------------------------------------------------------------
void IndexBuffer::SetData(void const* source, std::uint32_t elementCountIn)
{
	POMDOG_ASSERT(source != nullptr);
	POMDOG_ASSERT(elementCountIn > 0);
	POMDOG_ASSERT(elementCountIn <= indexCount);
	POMDOG_ASSERT(nativeIndexBuffer);
	POMDOG_ASSERT(bufferUsage != Pomdog::BufferUsage::Immutable);
	nativeIndexBuffer->SetData(0, source, ToIndexElementOffsetBytes(elementSize) * elementCountIn);
}
//-----------------------------------------------------------------------
void IndexBuffer::SetData(std::uint32_t offsetInBytes, void const* source, std::uint32_t elementCountIn)
{
	POMDOG_ASSERT(source != nullptr);
	POMDOG_ASSERT(elementCountIn > 0);
	POMDOG_ASSERT(elementCountIn <= indexCount);
	POMDOG_ASSERT(nativeIndexBuffer);
	POMDOG_ASSERT(bufferUsage != Pomdog::BufferUsage::Immutable);
	nativeIndexBuffer->SetData(offsetInBytes, source, ToIndexElementOffsetBytes(elementSize) * elementCountIn);
}
//-----------------------------------------------------------------------
Details::RenderSystem::NativeIndexBuffer* IndexBuffer::NativeIndexBuffer()
{
	POMDOG_ASSERT(nativeIndexBuffer);
	return nativeIndexBuffer.get();
}
//-----------------------------------------------------------------------
}// namespace Pomdog
