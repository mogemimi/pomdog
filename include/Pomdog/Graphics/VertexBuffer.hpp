//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_VERTEXBUFFER_3DDFD5CB_025A_40AE_9438_85D3064CE7D9_HPP
#define POMDOG_VERTEXBUFFER_3DDFD5CB_025A_40AE_9438_85D3064CE7D9_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <cstdint>
#include "../Config/Export.hpp"
#include "detail/ForwardDeclarations.hpp"
#include "VertexDeclaration.hpp"
#include "BufferUsage.hpp"

namespace Pomdog {
namespace Details {
namespace RenderSystem {

class NativeVertexBuffer;

}// namespace RenderSystem
}// namespace Details

/// @addtogroup Framework
/// @{
/// @addtogroup Graphics
/// @{

///@~Japanese
/// @brief 頂点バッファです。
class POMDOG_EXPORT VertexBuffer {
public:
	VertexBuffer() = delete;
	VertexBuffer(VertexBuffer const&) = delete;
	VertexBuffer(VertexBuffer &&) = default;

	VertexBuffer(std::shared_ptr<GraphicsDevice> const& graphicsDevice,
		Pomdog::VertexDeclaration const& vertexDeclaration,
		void const* vertices, std::uint32_t vertexCount, Pomdog::BufferUsage bufferUsage);

	VertexBuffer(std::shared_ptr<GraphicsDevice> const& graphicsDevice,
		Pomdog::VertexDeclaration && vertexDeclaration,
		void const* vertices, std::uint32_t vertexCount, Pomdog::BufferUsage bufferUsage);

	~VertexBuffer();

	VertexBuffer & operator=(VertexBuffer const&) = delete;
	VertexBuffer & operator=(VertexBuffer &&) = default;

	///@~Japanese
	/// @brief 頂点データの定義を取得します。
	VertexDeclaration const& VertexDeclaration() const;

	///@~Japanese
	/// @brief 頂点の数を取得します。
	std::uint32_t VertexCount() const;

	///@~Japanese
	/// @brief バッファの使用方法を取得します。
	Pomdog::BufferUsage BufferUsage() const;
	
	///@~Japanese
	/// @brief 頂点データを格納します。
	/// @param source ソースバッファを指定します。
	/// @param elementCount 頂点の数を指定します。
	void SetData(void const* source, std::uint32_t elementCount);
	
public:
	Details::RenderSystem::NativeVertexBuffer* NativeVertexBuffer();
	
private:
	Pomdog::VertexDeclaration vertexDeclaration;
	std::unique_ptr<Details::RenderSystem::NativeVertexBuffer> nativeVertexBuffer;
	std::uint32_t vertexCount;
	Pomdog::BufferUsage bufferUsage;
};

/// @}
/// @}

}// namespace Pomdog

#endif // !defined(POMDOG_VERTEXBUFFER_3DDFD5CB_025A_40AE_9438_85D3064CE7D9_HPP)
