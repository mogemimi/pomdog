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

///@~Japanese
/// @brief 頂点バッファです。
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

	///@~Japanese
	/// @brief 頂点の数を取得します。
	std::size_t VertexCount() const;

	///@~Japanese
	/// @brief 頂点データ 1 つ分のサイズをバイト単位で取得します。
	/// @remarks 1 つの頂点の先頭から次の頂点までのバイト数です。
	std::size_t StrideBytes() const;

	///@~Japanese
	/// @brief バッファの使用方法を取得します。
	Pomdog::BufferUsage BufferUsage() const;

	///@~Japanese
	/// @brief 頂点データを格納します。
	/// @param source ソースバッファ
	/// @param elementCount 頂点の数
	void SetData(void const* source, std::size_t elementCount);

	///@~Japanese
	/// @brief 頂点データを格納します。
	/// @param offsetInBytes バッファの先頭から出力先までのバイト単位のオフセット
	/// @param source ソースバッファ
	/// @param elementCount 頂点の数
	/// @param strideBytes 頂点 1 つ分のバイト単位のサイズ
	void SetData(std::size_t offsetInBytes, void const* source,
		std::size_t elementCount, std::size_t strideBytes);

public:
	Detail::RenderSystem::NativeVertexBuffer* NativeVertexBuffer();

private:
	std::unique_ptr<Detail::RenderSystem::NativeVertexBuffer> nativeVertexBuffer;
	std::uint32_t vertexCount;
	std::uint16_t strideBytes;
	Pomdog::BufferUsage bufferUsage;
};

}// namespace Pomdog

#endif // !defined(POMDOG_VERTEXBUFFER_3DDFD5CB_025A_40AE_9438_85D3064CE7D9_HPP)
