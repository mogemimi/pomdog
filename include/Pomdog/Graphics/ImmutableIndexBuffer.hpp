//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_IMMUTABLEINDEXBUFFER_AF678AFD_BB3D_4CD2_83FE_0CDE152AE545_HPP
#define POMDOG_IMMUTABLEINDEXBUFFER_AF678AFD_BB3D_4CD2_83FE_0CDE152AE545_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <memory>
#include "IndexBuffer.hpp"
#include "BufferUsage.hpp"
#include "IndexElementSize.hpp"

namespace Pomdog {
namespace Details {
namespace RenderSystem {

class NativeImmutableIndexBuffer;

}// namespace RenderSystem
}// namespace Details

/// @addtogroup Framework
/// @{
/// @addtogroup Graphics
/// @{

///@~Japanese
/// @brief 静的なインデックスバッファです。
class ImmutableIndexBuffer: public IndexBuffer
{
public:
	ImmutableIndexBuffer() = delete;
	ImmutableIndexBuffer(ImmutableIndexBuffer const&) = delete;
	ImmutableIndexBuffer(ImmutableIndexBuffer &&) = default;

	~ImmutableIndexBuffer() override;

	ImmutableIndexBuffer & operator=(ImmutableIndexBuffer const&) = delete;
	ImmutableIndexBuffer & operator=(ImmutableIndexBuffer &&) = default;

	///@~Japanese
	/// @brief インデックスの個数を取得します。
	std::size_t GetIndexCount() const;

	///@~Japanese
	/// @brief インデックスの要素 1 つ分のサイズを取得します。
	IndexElementSize GetElementSize() const;

	///@~Japanese
	/// @brief バッファの使用方法を取得します。
	BufferUsage GetBufferUsage() const;
	
public:
	Details::RenderSystem::NativeIndexBuffer* GetNativeIndexBuffer();
	
private:
	std::unique_ptr<Details::RenderSystem::NativeImmutableIndexBuffer> nativeIndexBuffer;
	std::size_t indexCount;
	IndexElementSize elementSize;
	BufferUsage bufferUsage;
};

/// @}
/// @}

}// namespace Pomdog

#endif // !defined(POMDOG_IMMUTABLEINDEXBUFFER_AF678AFD_BB3D_4CD2_83FE_0CDE152AE545_HPP)
