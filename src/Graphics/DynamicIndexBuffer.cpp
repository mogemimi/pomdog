//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include <Pomdog/Graphics/DynamicIndexBuffer.hpp>
#include <Pomdog/Utility/Assert.hpp>
#include <Pomdog/Graphics/BufferUsage.hpp>
#include <Pomdog/Graphics/GraphicsDevice.hpp>
#include "../RenderSystem/NativeGraphicsDevice.hpp"
#include "../RenderSystem/NativeIndexBuffer.hpp"

namespace Pomdog {
//-----------------------------------------------------------------------
DynamicIndexBuffer::DynamicIndexBuffer(std::shared_ptr<GraphicsDevice> const& graphicsDevice,
	IndexElementSize newElementSize, void const* indices, std::uint32_t newIndexCount)
	: nativeIndexBuffer(graphicsDevice->NativeGraphicsDevice()->CreateIndexBuffer(
		indices, newIndexCount, newElementSize, BufferUsage::Dynamic))
	, indexCount(newIndexCount)
	, elementSize(newElementSize)
{
	POMDOG_ASSERT(nativeIndexBuffer);
}
//-----------------------------------------------------------------------
DynamicIndexBuffer::~DynamicIndexBuffer() = default;
//-----------------------------------------------------------------------
std::uint32_t DynamicIndexBuffer::IndexCount() const
{
	return indexCount;
}
//-----------------------------------------------------------------------
IndexElementSize DynamicIndexBuffer::ElementSize() const
{
	return elementSize;
}
//-----------------------------------------------------------------------
BufferUsage DynamicIndexBuffer::BufferUsage() const
{
	return BufferUsage::Dynamic;
}
//-----------------------------------------------------------------------
void DynamicIndexBuffer::SetData(void const* source, std::uint32_t elementCount)
{
	POMDOG_ASSERT(source != nullptr);
	POMDOG_ASSERT(elementCount > 0);
	
	POMDOG_ASSERT(nativeIndexBuffer);
	nativeIndexBuffer->SetData(source, elementCount, elementSize);
}
//-----------------------------------------------------------------------
Details::RenderSystem::NativeIndexBuffer* DynamicIndexBuffer::NativeIndexBuffer()
{
	POMDOG_ASSERT(nativeIndexBuffer);
	return nativeIndexBuffer.get();
}
//-----------------------------------------------------------------------
}// namespace Pomdog
