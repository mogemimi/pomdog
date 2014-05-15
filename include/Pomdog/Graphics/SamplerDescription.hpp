//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_SAMPLERDESCRIPTION_CA2C90E4_B1C7_4E4F_816B_D56ACF3292D0_HPP
#define POMDOG_SAMPLERDESCRIPTION_CA2C90E4_B1C7_4E4F_816B_D56ACF3292D0_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <cstdint>
#include <limits>
#include "../Config/Export.hpp"
#include "TextureAddressMode.hpp"
#include "TextureFilter.hpp"

namespace Pomdog {

/// @addtogroup Framework
/// @{
/// @addtogroup Graphics
/// @{

///@~Japanese
/// @brief サンプラーステートの設定を記述します。
class POMDOG_EXPORT SamplerDescription {
public:
	///@~Japanese
	/// @brief 異方性フィルタリングで使用される最大異方性を表すクランプ値です。
	/// @remarks 異方性フィルタリングとして有効な値は 1 から 16 です。デフォルト値は 16 です。
	/// SamplerDescription::Filter に TextureFilter::Anisotropic を指定している時、この値が有効になります。
	std::uint32_t MaxAnisotropy = 0;

	///@~Japanese
	/// @brief アクセス可能な最も低解像度のミップマップレベルの最小値です。
	/// @remarks デフォルト値は std::numeric_limits<float>::min() です。
	float MinMipLevel = std::numeric_limits<float>::min();

	///@~Japanese
	/// @brief アクセス可能な最も低解像度のミップマップレベルの最大値です。
	/// ミップマップレベルが指定された値を超えた場合はクランプされます。
	/// デフォルト値は std::numeric_limits<float>::max() です。
	/// @remarks 必ず MinMipLevel より大きな値を設定してください。
	/// 0 を最も詳細なミップマップレベルとし、レベルの値が大きくなるほど詳細でなくなります。
	/// MaxMipLevel を 4 に設定すると、レベル 4 以上のミップマップへのアクセスをできなくすることができます。
	/// 0 よりも大きな値を指定する必要があります。
	/// LOD の上限を決めない場合は、非常に大きな値を指定します。
	/// @details
	/// 実際に指定される値は、実装によって異なります。
	/// 実装では LOD の上限を決めないように、大きな値が指定されています。
	/// GL の場合、GL_TEXTURE_MAX_LOD のデフォルト値は +1000U です。
	/// Direct3D の場合、D3D11_SAMPLER_DESC::MaxLOD のデフォルト値は D3D11_FLOAT32_MAX です。
	float MaxMipLevel = std::numeric_limits<float>::max();

	///@~Japanese
	/// @brief テクスチャサンプリング時に使用するフィルタリングオプションです。
	/// デフォルト値は TextureFilter::Linear です。
	TextureFilter Filter = TextureFilter::Linear;

	///@~Japanese
	/// @brief 0.0 から 1.0 の範囲外にあるテクスチャの u 座標を解決するために使用されるテクスチャアドレッシングモードです。
	/// デフォルト値は TextureAddressMode::Clamp です。
	TextureAddressMode AddressU = TextureAddressMode::Clamp;

	///@~Japanese
	/// @brief 0.0 から 1.0 の範囲外にあるテクスチャの v 座標を解決するために使用されるテクスチャアドレッシングモードです。
	/// デフォルト値は TextureAddressMode::Clamp です。
	TextureAddressMode AddressV = TextureAddressMode::Clamp;

	///@~Japanese
	/// @brief 0.0 から 1.0 の範囲外にあるテクスチャの w 座標を解決するために使用されるテクスチャアドレッシングモードです。
	/// デフォルト値は TextureAddressMode::Clamp です。
	TextureAddressMode AddressW = TextureAddressMode::Clamp;
	
	//Color BorderColor = Color::TransparentBlack;
	//float MipMapLevelOfDetailBias = 0;
};

/// @}
/// @}

}// namespace Pomdog

#endif // !defined(POMDOG_SAMPLERDESCRIPTION_CA2C90E4_B1C7_4E4F_816B_D56ACF3292D0_HPP)
