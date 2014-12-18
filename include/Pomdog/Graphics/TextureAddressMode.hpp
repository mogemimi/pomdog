//
//  Copyright (C) 2013-2014 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_TEXTUREADDRESSMODE_256FCFEC_6DE0_40B1_9326_E113017D0F57_HPP
#define POMDOG_TEXTUREADDRESSMODE_256FCFEC_6DE0_40B1_9326_E113017D0F57_HPP

#if (_MSC_VER > 1000)
#pragma once
#endif

#include <cstdint>

namespace Pomdog {

///@~Japanese
/// @brief テクスチャの境界である 0.0 から 1.0 の範囲外にあるテクスチャ座標のアドレッシング方法を定義します。
enum class TextureAddressMode: std::uint8_t {
	///@~Japanese
	/// @brief 整数境界ごとにテクスチャを繰り返します。
	/// @note See also:
	/// http://msdn.microsoft.com/ja-jp/library/bb206366(v=vs.85).aspx
	Wrap,

	///@~Japanese
	/// @brief 整数境界ごとにテクスチャをミラーリングします。
	/// @note See also:
	/// http://msdn.microsoft.com/ja-jp/library/bb147201(v=vs.85).aspx
	Mirror,
	
	///@~Japanese
	/// @brief テクスチャ座標を [0.0, 1.0] の範囲にクランプします。
	/// @note See also:
	/// http://msdn.microsoft.com/ja-jp/library/bb204826(v=vs.85).aspx
	Clamp,
};

}// namespace Pomdog

#endif // !defined(POMDOG_TEXTUREADDRESSMODE_256FCFEC_6DE0_40B1_9326_E113017D0F57_HPP)
