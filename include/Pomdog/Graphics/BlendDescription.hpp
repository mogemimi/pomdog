//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_BLENDDESCRIPTION_0C296679_CD00_43E5_8EFC_6AE11478FB02_HPP
#define POMDOG_BLENDDESCRIPTION_0C296679_CD00_43E5_8EFC_6AE11478FB02_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include "Blend.hpp"
#include "BlendFunction.hpp"
#include "Pomdog/Math/Color.hpp"
#include "Pomdog/Basic/Export.hpp"
#include <cstdint>
#include <limits>

namespace Pomdog {

///@~Japanese
/// @brief ブレンディング処理を記述します。
class POMDOG_EXPORT BlendDescription {
public:
	///@~Japanese
	/// @brief ブレンディング係数を指定します。デフォルト値は Color::White です。
	/// @note 使用するにはブレンディング係数に Blend::BlendFactor
	/// または Blend::InvereseBlendFactor を指定する必要があります。
	Color BlendFactor = Color::White;

	///@~Japanese
	/// @brief マルチサンプリング時の書き込み位置を指定したビットマスクです。
	/// デフォルト値は std::numeric_limits<std::uint32_t>::max() です。
	/// @details デフォルト値は、全てのビットが 1 になっている符号なし整数の最大値です。
	/// 実装については ID3D11DeviceContext::OMSetBlendState を参照ください。
	/// @remarks 32 ビットのビットマスクです。各ビットが各チャンネルに対応しています。
	/// 例えば、次のようにを指定します。
	/// R チャンネルのみ書き込む場合は、0x01。
	/// GB チャンネルのみ書き込む場合は (0x02 | 0x04)。
	/// RGB チャンネルを有効化する場合、(0x01 | 0x02 | 0x04)。
	/// RGBA 全てを有効化する場合は (0x01 | 0x02 | 0x04 | 0x08)。
	/// また、マルチサンプリングを行わない場合は、指定する必要はありません。
	std::uint32_t MultiSampleMask = std::numeric_limits<std::uint32_t>::max();

	///@~Japanese
	/// @brief RGB カラーのブレンディングの組み合わせ方法を指定します。
	/// デフォルト値は BlendFunction::Add です。
	BlendFunction ColorBlendFunction = BlendFunction::Add;

	///@~Japanese
	/// @brief アルファ値のブレンディングの組み合わせ方法を指定します。
	/// デフォルト値は BlendFunction::Add です。
	BlendFunction AlphaBlendFunction = BlendFunction::Add;

	///@~Japanese
	/// @brief ソースカラーのブレンディング係数を指定します。
	/// デフォルト値は Blend::One です。
	Blend ColorSourceBlend = Blend::One;

	///@~Japanese
	/// @brief ソースアルファのブレンディング係数を指定します。
	/// デフォルト値は Blend::One です。
	Blend AlphaSourceBlend = Blend::One;

	///@~Japanese
	/// @brief ディスティネーションカラーのブレンディング係数を指定します。
	/// デフォルト値は Blend::One です。
	Blend ColorDestinationBlend = Blend::One;

	///@~Japanese
	/// @brief ディスティネーションアルファのブレンディング係数を指定します。
	/// デフォルト値は Blend::One です。
	Blend AlphaDestinationBlend = Blend::One;

	///@~Japanese
	/// @brief マルチサンプリングテクニックとしてアルファトゥカバレッジを使用するかどうか指定します。
	/// デフォルト値はアルファトゥカバレッジが無効 (false) になっています。
	/// @remarks アルファトゥカバレッジを有効にする場合は、true を、無効にする場合は false を指定します。
	bool AlphaToCoverageEnable = false;
};

}// namespace Pomdog

#endif // !defined(POMDOG_BLENDDESCRIPTION_0C296679_CD00_43E5_8EFC_6AE11478FB02_HPP)
