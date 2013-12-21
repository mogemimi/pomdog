//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include <Pomdog/Graphics/ImmutableIndexBuffer.hpp>
#include <Pomdog/Utility/Assert.hpp>
#include <Pomdog/Graphics/GraphicsDevice.hpp>
#include "../RenderSystem/NativeGraphicsDevice.hpp"
#include "../RenderSystem/NativeIndexBuffer.hpp"
#include "BufferUsage.hpp"

namespace Pomdog {
//-----------------------------------------------------------------------
ImmutableIndexBuffer::ImmutableIndexBuffer(std::shared_ptr<GraphicsDevice> const& graphicsDevice,
	IndexElementSize newElementSize, void const* indices, std::uint32_t newIndexCount)
	: nativeIndexBuffer(graphicsDevice->GetNativeGraphicsDevice()->CreateIndexBuffer(
		indices, newIndexCount, newElementSize, BufferUsage::Immutable))
	, indexCount(newIndexCount)
	, elementSize(newElementSize)
{
	POMDOG_ASSERT(nativeIndexBuffer);
}
//-----------------------------------------------------------------------
ImmutableIndexBuffer::~ImmutableIndexBuffer()
{
}
//-----------------------------------------------------------------------
std::uint32_t ImmutableIndexBuffer::GetIndexCount() const
{
	return indexCount;
}
//-----------------------------------------------------------------------
IndexElementSize ImmutableIndexBuffer::GetElementSize() const
{
	return elementSize;
}
//-----------------------------------------------------------------------
BufferUsage ImmutableIndexBuffer::GetBufferUsage() const
{
	return BufferUsage::Immutable;
}
//-----------------------------------------------------------------------
Details::RenderSystem::NativeIndexBuffer* ImmutableIndexBuffer::GetNativeIndexBuffer()
{
	POMDOG_ASSERT(nativeIndexBuffer);
	return nativeIndexBuffer.get();
}
//-----------------------------------------------------------------------
}// namespace Pomdog
