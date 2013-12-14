//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include <Pomdog/Graphics/DynamicIndexBuffer.hpp>
#include <Pomdog/Utility/Assert.hpp>
#include <Pomdog/Graphics/GraphicsDevice.hpp>
#include "../RenderSystem/NativeGraphicsDevice.hpp"
#include "../RenderSystem/NativeIndexBuffer.hpp"
#include "BufferUsage.hpp"

namespace Pomdog {
//-----------------------------------------------------------------------
DynamicIndexBuffer::DynamicIndexBuffer(std::shared_ptr<GraphicsDevice> const& graphicsDevice,
	IndexElementSize newElementSize, void const* indices, std::size_t newIndexCount)
	: nativeIndexBuffer(graphicsDevice->GetNativeGraphicsDevice()->CreateIndexBuffer(
		indices, newIndexCount, newElementSize, BufferUsage::Dynamic))
	, indexCount(newIndexCount)
	, elementSize(newElementSize)
{
	POMDOG_ASSERT(nativeIndexBuffer);
}
//-----------------------------------------------------------------------
DynamicIndexBuffer::~DynamicIndexBuffer()
{
}
//-----------------------------------------------------------------------
std::size_t DynamicIndexBuffer::GetIndexCount() const
{
	return indexCount;
}
//-----------------------------------------------------------------------
IndexElementSize DynamicIndexBuffer::GetElementSize() const
{
	return elementSize;
}
//-----------------------------------------------------------------------
BufferUsage DynamicIndexBuffer::GetBufferUsage() const
{
	return BufferUsage::Dynamic;
}
//-----------------------------------------------------------------------
void DynamicIndexBuffer::SetData(void const* source, std::size_t indexCount)
{
	POMDOG_ASSERT(source != nullptr);
	POMDOG_ASSERT(indexCount > 0);
	
	POMDOG_ASSERT(nativeIndexBuffer);
	nativeIndexBuffer->SetData(source, indexCount, elementSize);
}
//-----------------------------------------------------------------------
Details::RenderSystem::NativeIndexBuffer* DynamicIndexBuffer::GetNativeIndexBuffer()
{
	POMDOG_ASSERT(nativeIndexBuffer);
	return nativeIndexBuffer.get();
}
//-----------------------------------------------------------------------
}// namespace Pomdog
