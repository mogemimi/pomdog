//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_DYNAMICINDEXBUFFER_D49044F7_5ADC_43E5_87AD_6FE1EA564050_HPP
#define POMDOG_DYNAMICINDEXBUFFER_D49044F7_5ADC_43E5_87AD_6FE1EA564050_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <memory>
#include "detail/ForwardDeclarations.hpp"
#include "IndexBuffer.hpp"
#include "IndexElementSize.hpp"

namespace Pomdog {

/// @addtogroup Framework
/// @{
/// @addtogroup Graphics
/// @{

///@~Japanese
/// @brief 動的なインデックスバッファです。
class DynamicIndexBuffer: public IndexBuffer
{
public: 
	DynamicIndexBuffer() = delete;
	DynamicIndexBuffer(DynamicIndexBuffer const&) = delete;
	DynamicIndexBuffer(DynamicIndexBuffer &&) = default;

	DynamicIndexBuffer(std::shared_ptr<GraphicsDevice> const& graphicsDevice,
		IndexElementSize elementSize, void const* indices, std::size_t indexCount);

	~DynamicIndexBuffer() override;

	DynamicIndexBuffer & operator=(DynamicIndexBuffer const&) = delete;
	DynamicIndexBuffer & operator=(DynamicIndexBuffer &&) = default;

	///@~Japanese
	/// @brief インデックスの個数を取得します。
	std::size_t GetIndexCount() const override;

	///@~Japanese
	/// @brief インデックスの要素 1 つ分のサイズを取得します。
	IndexElementSize GetElementSize() const override;

	///@~Japanese
	/// @brief バッファの使用方法を取得します。
	BufferUsage GetBufferUsage() const override;

	///@~Japanese
	/// @brief インデックスデータを格納します。
	/// @param source ソースバッファを指定します。
	/// @param indexCount インデックスの要素数を指定します。
	void SetData(void const* source, std::size_t indexCount);
	
public:
	Details::RenderSystem::NativeIndexBuffer* GetNativeIndexBuffer() override;
	
private:
	std::unique_ptr<Details::RenderSystem::NativeIndexBuffer> nativeIndexBuffer;
	std::size_t indexCount;
	IndexElementSize elementSize;
};

/// @}
/// @}

}// namespace Pomdog

#endif // !defined(POMDOG_DYNAMICINDEXBUFFER_D49044F7_5ADC_43E5_87AD_6FE1EA564050_HPP)
