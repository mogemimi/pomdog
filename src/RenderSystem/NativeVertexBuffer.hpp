//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_NATIVEVERTEXBUFFER_37AE3D68_01A8_4D42_83CF_C2EE0D92AC91_HPP
#define POMDOG_NATIVEVERTEXBUFFER_37AE3D68_01A8_4D42_83CF_C2EE0D92AC91_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include <cstdint>

namespace Pomdog {
namespace Detail {
namespace RenderSystem {

class NativeVertexBuffer {
public:
	NativeVertexBuffer() = default;
	NativeVertexBuffer(NativeVertexBuffer const&) = delete;
	NativeVertexBuffer & operator=(NativeVertexBuffer const&) = delete;

	virtual ~NativeVertexBuffer() = default;

	///@~Japanese
	/// @brief 頂点データを格納します。
	/// @remarks イミュータブルな頂点バッファでは使用しないでください。
	/// @param offsetInBytes バッファの先頭から出力先までのバイト単位のオフセット
	/// @param source ソースバッファ
	/// @param sizeInBytes 頂点データのバイト単位のサイズ
	virtual void SetData(std::uint32_t offsetInBytes,
		void const* source, std::uint32_t sizeInBytes) = 0;
};

}// namespace RenderSystem
}// namespace Detail
}// namespace Pomdog

#endif // !defined(POMDOG_NATIVEVERTEXBUFFER_37AE3D68_01A8_4D42_83CF_C2EE0D92AC91_HPP)
