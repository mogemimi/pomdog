//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_TEXTURE2D_F041ED3F_9089_4056_BC4F_1C145612778F_HPP
#define POMDOG_TEXTURE2D_F041ED3F_9089_4056_BC4F_1C145612778F_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include "../Config/Export.hpp"
#include "../Config/FundamentalTypes.hpp"
#include "SurfaceFormat.hpp"

namespace Pomdog {

/// @addtogroup Framework
/// @{
/// @addtogroup Graphics
/// @{

///@~Japanese
/// @brief 2 次元テクスチャです。
class POMDOG_EXPORT Texture2D //: public Texture
{
public: 
	~Texture2D();

	///@~Japanese
	/// @brief テクスチャの水平方向の幅（ピクセル単位）を取得します。
	std::uint32_t GetWidth() const;

	///@~Japanese
	/// @brief テクスチャの垂直方向の幅（ピクセル単位）を取得します。
	std::uint32_t GetHeight() const;

	///@~Japanese
	/// @brief テクスチャのフォーマットを取得します。
	SurfaceFormat GetFormat() const;

	///@~Japanese
	/// @brief ミップマップレベルを取得します。
	/// @remarks ミップマップレベルは 1 以上を有効とします。
	std::uint32_t GetLevelCount() const;
};

/// @}
/// @}

}// namespace Pomdog

#endif // !defined(POMDOG_TEXTURE2D_F041ED3F_9089_4056_BC4F_1C145612778F_HPP)
