//
//  Copyright (C) 2013-2014 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_TEXTUREFILTER_129A8A36_0F05_4EEC_A6AC_4026337AB617_HPP
#define POMDOG_TEXTUREFILTER_129A8A36_0F05_4EEC_A6AC_4026337AB617_HPP

#if (_MSC_VER > 1000)
#pragma once
#endif

#include <cstdint>

namespace Pomdog {

///@~Japanese
/// @brief テクスチャサンプリング時のフィルタリングオプションを定義します。
enum class TextureFilter: std::uint8_t {
	///@~Japanese
	/// @brief 縮小、拡大およびミップレベルのサンプリングに線形フィルタリングを使用します。
	Linear,

	///@~Japanese
	/// @brief 縮小、拡大およびミップレベルのサンプリングにポイントフィルタリングを使用します。
	Point,

	///@~Japanese
	/// @brief 縮小、拡大およびミップレベルのサンプリングに異方性フィルタリングを使用します。
	Anisotropic,

	///@~Japanese
	/// @brief 縮小および拡大サンプリングに線形フィルタリングを使用し、ミップレベルにポイントフィルタリングを使用します。
	LinearMipPoint,

	///@~Japanese
	/// @brief 縮小および拡大サンプリングにポイントフィルタリングを使用し、ミップレベルに線形補完を使用します。
	PointMipLinear,

	///@~Japanese
	/// @brief 縮小およびミップレベルに線形フィルタリングを使用し、拡大サンプリングにポイントフィルタリングを使用します。
	MinLinearMagPointMipLinear,

	///@~Japanese
	/// @brief 拡大およびミップレベルにポイントフィルタリングを使用し、縮小サンプリングに線形フィルタリングを使用します。
	MinLinearMagPointMipPoint,

	///@~Japanese
	/// @brief 拡大およびミップレベルに線形フィルタリングを使用し、縮小サンプリングにポイントフィルタリングを使用します。
	MinPointMagLinearMipLinear,

	///@~Japanese
	/// @brief 縮小およびミップレベルにポイントフィルタリングを使用し、拡大サンプリングに線形フィルタリングを使用します。
	MinPointMagLinearMipPoint,
};

}// namespace Pomdog

#endif // !defined(POMDOG_TEXTUREFILTER_129A8A36_0F05_4EEC_A6AC_4026337AB617_HPP)
