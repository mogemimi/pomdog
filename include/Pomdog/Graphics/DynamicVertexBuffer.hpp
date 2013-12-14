//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_DYNAMICVERTEXBUFFER_A2CEAA26_EDB4_40C8_B035_ADEEC90F7EA7_HPP
#define POMDOG_DYNAMICVERTEXBUFFER_A2CEAA26_EDB4_40C8_B035_ADEEC90F7EA7_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <memory>
#include "VertexBuffer.hpp"
#include "BufferUsage.hpp"
#include "VertexDeclaration.hpp"

namespace Pomdog {

/// @addtogroup Framework
/// @{
/// @addtogroup Graphics
/// @{

///@~Japanese
/// @brief 動的な頂点バッファです。
class DynamicVertexBuffer: public VertexBuffer
{
public:
	DynamicVertexBuffer() = delete;
	DynamicVertexBuffer(DynamicVertexBuffer const&) = delete;
	DynamicVertexBuffer(DynamicVertexBuffer &&) = default;

	~DynamicVertexBuffer() override;

	DynamicVertexBuffer & operator=(DynamicVertexBuffer const&) = delete;
	DynamicVertexBuffer & operator=(DynamicVertexBuffer &&) = default;

	///@~Japanese
	/// @brief 頂点データの定義を取得します。
	VertexDeclaration const& GetVertexDeclaration() const;

	///@~Japanese
	/// @brief バッファの使用方法を取得します。
	BufferUsage GetBufferUsage() const;

	///@~Japanese
	/// @brief 頂点の数を取得します。
	std::size_t GetVertexCount() const;

	///@~Japanese
	/// @brief 頂点データを格納します。
	/// @param source ソースバッファを指定します。
	/// @param vertexCount 頂点の数を指定します。
	void SetData(void const* source, std::size_t vertexCount);
	
public:
	Details::RenderSystem::NativeVertexBuffer* GetNativeVertexBuffer();

private:
	std::unique_ptr<Details::RenderSystem::NativeVertexBuffer> nativeVertexBuffer;
	VertexDeclaration vertexDeclaration;
	std::size_t vertexCount;
	BufferUsage bufferUsage;
};

/// @}
/// @}

}// namespace Pomdog

#endif // !defined(POMDOG_DYNAMICVERTEXBUFFER_A2CEAA26_EDB4_40C8_B035_ADEEC90F7EA7_HPP)
