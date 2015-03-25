// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_VERTEXBUFFER_3DDFD5CB_025A_40AE_9438_85D3064CE7D9_HPP
#define POMDOG_VERTEXBUFFER_3DDFD5CB_025A_40AE_9438_85D3064CE7D9_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include "detail/ForwardDeclarations.hpp"
#include "BufferUsage.hpp"
#include "Pomdog/Basic/Export.hpp"
#include <cstdint>
#include <memory>

namespace Pomdog {
namespace Detail {
namespace RenderSystem {

class NativeVertexBuffer;

}// namespace RenderSystem
}// namespace Detail

class POMDOG_EXPORT VertexBuffer {
public:
	VertexBuffer() = delete;
	VertexBuffer(VertexBuffer const&) = delete;
	VertexBuffer(VertexBuffer &&) = default;

	VertexBuffer(GraphicsDevice & graphicsDevice,
		void const* vertices, std::size_t vertexCount,
		std::size_t strideBytes, Pomdog::BufferUsage bufferUsage);

	VertexBuffer(GraphicsDevice & graphicsDevice,
		std::size_t vertexCount, std::size_t strideBytes,
		Pomdog::BufferUsage bufferUsage);

	VertexBuffer(std::shared_ptr<GraphicsDevice> const& graphicsDevice,
		void const* vertices, std::size_t vertexCount,
		std::size_t strideBytes, Pomdog::BufferUsage bufferUsage);

	VertexBuffer(std::shared_ptr<GraphicsDevice> const& graphicsDevice,
		std::size_t vertexCount, std::size_t strideBytes,
		Pomdog::BufferUsage bufferUsage);

	~VertexBuffer();

	VertexBuffer & operator=(VertexBuffer const&) = delete;
	VertexBuffer & operator=(VertexBuffer &&) = default;

	std::size_t VertexCount() const;

	std::size_t StrideBytes() const;

	Pomdog::BufferUsage BufferUsage() const;

	void SetData(void const* source, std::size_t elementCount);

	void SetData(std::size_t offsetInBytes, void const* source,
		std::size_t elementCount, std::size_t strideBytes);

public:
	Detail::RenderSystem::NativeVertexBuffer* NativeVertexBuffer();

private:
	std::unique_ptr<Detail::RenderSystem::NativeVertexBuffer> nativeVertexBuffer;
	std::uint32_t vertexCount;
	std::uint32_t strideBytes;
	Pomdog::BufferUsage bufferUsage;
};

}// namespace Pomdog

#endif // !defined(POMDOG_VERTEXBUFFER_3DDFD5CB_025A_40AE_9438_85D3064CE7D9_HPP)
