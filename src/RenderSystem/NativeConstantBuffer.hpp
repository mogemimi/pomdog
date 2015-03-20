//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_NATIVECONSTANTBUFFER_72EA6CD2_41A8_475D_B0B3_7A04BD051A66_HPP
#define POMDOG_NATIVECONSTANTBUFFER_72EA6CD2_41A8_475D_B0B3_7A04BD051A66_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include <cstdint>

namespace Pomdog {
namespace Detail {
namespace RenderSystem {

class NativeConstantBuffer {
public:
	NativeConstantBuffer() = default;
	NativeConstantBuffer(NativeConstantBuffer const&) = delete;
	NativeConstantBuffer & operator=(NativeConstantBuffer const&) = delete;

	virtual ~NativeConstantBuffer() = default;

	///@copydoc ConstantBuffer
	virtual void GetData(std::uint32_t byteWidth, void* result) const = 0;

	///@~Japanese
	/// @brief 定数バッファデータを格納します。
	/// @remarks イミュータブルな定数バッファでは使用しないでください。
	/// @param offsetInBytes バッファの先頭から出力先までのバイト単位のオフセット
	/// @param source ソースバッファ
	/// @param sizeInBytes 頂点データのバイト単位のサイズ
	virtual void SetData(std::uint32_t offsetInBytes,
		void const* source, std::uint32_t sizeInBytes) = 0;
};

}// namespace RenderSystem
}// namespace Detail
}// namespace Pomdog

#endif // !defined(POMDOG_NATIVECONSTANTBUFFER_72EA6CD2_41A8_475D_B0B3_7A04BD051A66_HPP)
