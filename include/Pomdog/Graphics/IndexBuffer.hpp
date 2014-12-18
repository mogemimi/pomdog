//
//  Copyright (C) 2013-2014 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_INDEXBUFFER_4119FC34_308A_4EF7_BC78_92FD2D6DBC43_HPP
#define POMDOG_INDEXBUFFER_4119FC34_308A_4EF7_BC78_92FD2D6DBC43_HPP

#if (_MSC_VER > 1000)
#pragma once
#endif

#include "detail/ForwardDeclarations.hpp"
#include "BufferUsage.hpp"
#include "IndexElementSize.hpp"
#include "Pomdog/Config/Export.hpp"
#include <cstdint>
#include <memory>

namespace Pomdog {
namespace Details {
namespace RenderSystem {

class NativeIndexBuffer;

}// namespace RenderSystem
}// namespace Details

///@~Japanese
/// @brief インデックスバッファです。
class POMDOG_EXPORT IndexBuffer {
public:
	IndexBuffer() = delete;
	IndexBuffer(IndexBuffer const&) = delete;
	IndexBuffer(IndexBuffer &&) = default;

	IndexBuffer(GraphicsDevice & graphicsDevice,
		IndexElementSize elementSize, void const* indices, std::uint32_t indexCount,
		Pomdog::BufferUsage bufferUsage);
	
	IndexBuffer(GraphicsDevice & graphicsDevice,
		IndexElementSize elementSize, std::uint32_t indexCount,
		Pomdog::BufferUsage bufferUsage);

	IndexBuffer(std::shared_ptr<GraphicsDevice> const& graphicsDevice,
		IndexElementSize elementSize, void const* indices, std::uint32_t indexCount,
		Pomdog::BufferUsage bufferUsage);
	
	IndexBuffer(std::shared_ptr<GraphicsDevice> const& graphicsDevice,
		IndexElementSize elementSize, std::uint32_t indexCount,
		Pomdog::BufferUsage bufferUsage);

	~IndexBuffer();

	IndexBuffer & operator=(IndexBuffer const&) = delete;
	IndexBuffer & operator=(IndexBuffer &&) = default;

	///@~Japanese
	/// @brief インデックスの個数を取得します。
	std::uint32_t IndexCount() const;

	///@~Japanese
	/// @brief インデックスの要素 1 つ分のサイズを取得します。
	IndexElementSize ElementSize() const;

	///@~Japanese
	/// @brief バッファの使用方法を取得します。
	Pomdog::BufferUsage BufferUsage() const;

	///@~Japanese
	/// @brief インデックスデータを格納します。
	/// @param source ソースバッファ
	/// @param elementCount インデックスの要素数
	void SetData(void const* source, std::uint32_t elementCount);

	///@~Japanese
	/// @brief インデックスデータを格納します。
	/// @param offsetInBytes バッファの先頭から出力先までのバイト単位のオフセット
	/// @param source ソースバッファ
	/// @param elementCount インデックスの要素数
	void SetData(std::uint32_t offsetInBytes, void const* source, std::uint32_t elementCount);

public:
	Details::RenderSystem::NativeIndexBuffer* NativeIndexBuffer();
	
private:
	std::unique_ptr<Details::RenderSystem::NativeIndexBuffer> nativeIndexBuffer;
	std::uint32_t indexCount;
	IndexElementSize elementSize;
	Pomdog::BufferUsage bufferUsage;
};

}// namespace Pomdog

#endif // !defined(POMDOG_INDEXBUFFER_4119FC34_308A_4EF7_BC78_92FD2D6DBC43_HPP)
