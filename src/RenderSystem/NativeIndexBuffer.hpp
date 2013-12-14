//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_SRC_RENDERSYSTEM_NATIVEINDEXBUFFER_C6FFE20E_160B_4B28_BD28_4F4A509BEC47_HPP
#define POMDOG_SRC_RENDERSYSTEM_NATIVEINDEXBUFFER_C6FFE20E_160B_4B28_BD28_4F4A509BEC47_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <Pomdog/Config/FundamentalTypes.hpp>
#include <Pomdog/Graphics/detail/ForwardDeclarations.hpp>

namespace Pomdog {
namespace Details {
namespace RenderSystem {

class NativeIndexBuffer
{
public:
	NativeIndexBuffer() = default;
	NativeIndexBuffer(NativeIndexBuffer const&) = delete;
	NativeIndexBuffer& operator=(NativeIndexBuffer const&) = delete;

	virtual ~NativeIndexBuffer() = default;
	
	///@~Japanese
	/// @brief インデックスデータを格納します。
	/// @remarks イミュータブルなインデックスバッファでは使用しないでください。
	/// @param source ソースバッファを指定します。
	/// @param indexCount インデックスの要素数を指定します。
	virtual void SetData(void const* source, std::size_t indexCount, IndexElementSize elementSize) = 0;
};

}// namespace RenderSystem
}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_SRC_RENDERSYSTEM_NATIVEINDEXBUFFER_C6FFE20E_160B_4B28_BD28_4F4A509BEC47_HPP)
