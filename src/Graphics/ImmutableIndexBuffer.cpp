//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include <Pomdog/Graphics/ImmutableIndexBuffer.hpp>
#include <Pomdog/Utility/Assert.hpp>
#include <Pomdog/Graphics/BufferUsage.hpp>
#include <Pomdog/Graphics/GraphicsDevice.hpp>
#include "../RenderSystem/NativeGraphicsDevice.hpp"
#include "../RenderSystem/NativeIndexBuffer.hpp"

namespace Pomdog {
//-----------------------------------------------------------------------
ImmutableIndexBuffer::ImmutableIndexBuffer(std::shared_ptr<GraphicsDevice> const& graphicsDevice,
	IndexElementSize newElementSize, void const* indices, std::uint32_t newIndexCount)
	: nativeIndexBuffer(graphicsDevice->NativeGraphicsDevice()->CreateIndexBuffer(
		indices, newIndexCount, newElementSize, BufferUsage::Immutable))
	, indexCount(newIndexCount)
	, elementSize(newElementSize)
{
	POMDOG_ASSERT(nativeIndexBuffer);
}
//-----------------------------------------------------------------------
ImmutableIndexBuffer::~ImmutableIndexBuffer() = default;
//-----------------------------------------------------------------------
std::uint32_t ImmutableIndexBuffer::IndexCount() const
{
	return indexCount;
}
//-----------------------------------------------------------------------
IndexElementSize ImmutableIndexBuffer::ElementSize() const
{
	return elementSize;
}
//-----------------------------------------------------------------------
BufferUsage ImmutableIndexBuffer::BufferUsage() const
{
	return BufferUsage::Immutable;
}
//-----------------------------------------------------------------------
Details::RenderSystem::NativeIndexBuffer* ImmutableIndexBuffer::NativeIndexBuffer()
{
	POMDOG_ASSERT(nativeIndexBuffer);
	return nativeIndexBuffer.get();
}
//-----------------------------------------------------------------------
}// namespace Pomdog
