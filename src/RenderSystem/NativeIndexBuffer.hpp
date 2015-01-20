//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_NATIVEINDEXBUFFER_C6FFE20E_160B_4B28_BD28_4F4A509BEC47_HPP
#define POMDOG_NATIVEINDEXBUFFER_C6FFE20E_160B_4B28_BD28_4F4A509BEC47_HPP

#if (_MSC_VER > 1000)
#pragma once
#endif

#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include <cstdint>

namespace Pomdog {
namespace Details {
namespace RenderSystem {

class NativeIndexBuffer {
public:
	NativeIndexBuffer() = default;
	NativeIndexBuffer(NativeIndexBuffer const&) = delete;
	NativeIndexBuffer & operator=(NativeIndexBuffer const&) = delete;

	virtual ~NativeIndexBuffer() = default;

	///@~Japanese
	/// @brief インデックスデータを格納します。
	/// @remarks イミュータブルなインデックスバッファでは使用しないでください。
	/// @param offsetInBytes バッファの先頭から出力先までのバイト単位のオフセット
	/// @param source ソースバッファ
	/// @param sizeInBytes インデックスデータのバイト単位のサイズ
	virtual void SetData(std::uint32_t offsetInBytes,
		void const* source, std::uint32_t sizeInBytes) = 0;
};

}// namespace RenderSystem
}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_NATIVEINDEXBUFFER_C6FFE20E_160B_4B28_BD28_4F4A509BEC47_HPP)
