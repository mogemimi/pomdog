//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include <Pomdog/Graphics/DynamicVertexBuffer.hpp>
#include <utility>
#include <Pomdog/Utility/Assert.hpp>
#include <Pomdog/Graphics/BufferUsage.hpp>
#include <Pomdog/Graphics/GraphicsDevice.hpp>
#include "../RenderSystem/NativeGraphicsDevice.hpp"
#include "../RenderSystem/NativeVertexBuffer.hpp"

namespace Pomdog {
//-----------------------------------------------------------------------
DynamicVertexBuffer::DynamicVertexBuffer(std::shared_ptr<GraphicsDevice> const& graphicsDevice,
	VertexDeclaration const& newVertexDeclaration, void const* vertices, std::uint32_t newVertexCount)
	: vertexDeclaration(newVertexDeclaration)
	, nativeVertexBuffer(graphicsDevice->GetNativeGraphicsDevice()->CreateVertexBuffer(
		vertices, newVertexCount, vertexDeclaration, BufferUsage::Dynamic))
	, vertexCount(newVertexCount)
{
	POMDOG_ASSERT(nativeVertexBuffer);
}
//-----------------------------------------------------------------------
DynamicVertexBuffer::DynamicVertexBuffer(std::shared_ptr<GraphicsDevice> const& graphicsDevice,
	VertexDeclaration && newVertexDeclaration, void const* vertices, std::uint32_t newVertexCount)
	: vertexDeclaration(std::move(newVertexDeclaration))
	, nativeVertexBuffer(graphicsDevice->GetNativeGraphicsDevice()->CreateVertexBuffer(
		vertices, newVertexCount, vertexDeclaration, BufferUsage::Dynamic))
	, vertexCount(newVertexCount)
{
	POMDOG_ASSERT(nativeVertexBuffer);
}
//-----------------------------------------------------------------------
DynamicVertexBuffer::~DynamicVertexBuffer()
{
}
//-----------------------------------------------------------------------
VertexDeclaration const& DynamicVertexBuffer::GetVertexDeclaration() const
{
	return vertexDeclaration;
}
//-----------------------------------------------------------------------
BufferUsage DynamicVertexBuffer::GetBufferUsage() const
{
	return BufferUsage::Dynamic;
}
//-----------------------------------------------------------------------
std::uint32_t DynamicVertexBuffer::GetVertexCount() const
{
	return vertexCount;
}
//-----------------------------------------------------------------------
void DynamicVertexBuffer::SetData(void const* source, std::uint32_t vertexCount)
{
	POMDOG_ASSERT(source != nullptr);
	POMDOG_ASSERT(vertexCount > 0);

	POMDOG_ASSERT(nativeVertexBuffer);
	nativeVertexBuffer->SetData(source, vertexCount, vertexDeclaration);
}
//-----------------------------------------------------------------------
Details::RenderSystem::NativeVertexBuffer* DynamicVertexBuffer::GetNativeVertexBuffer()
{
	POMDOG_ASSERT(nativeVertexBuffer);
	return nativeVertexBuffer.get();
}
//-----------------------------------------------------------------------
}// namespace Pomdog
