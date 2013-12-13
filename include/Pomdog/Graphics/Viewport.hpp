//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_VIEWPORT_AA637108_9222_46B7_BC9D_293048246897_HPP
#define POMDOG_VIEWPORT_AA637108_9222_46B7_BC9D_293048246897_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include "../Config/Export.hpp"
#include "../Config/FundamentalTypes.hpp"
#include "../Math/Rectangle.hpp"

namespace Pomdog {

class Rectangle;

/// @addtogroup Framework
/// @{
/// @addtogroup Graphics
/// @{

///@~Japanese
/// @brief ビューポートです。
class POMDOG_EXPORT Viewport
{
public:
	Rectangle bounds;
	float minDepth;
	float maxDepth;

public:
	Viewport() = default;
	explicit Viewport(Rectangle const& bounds);
	Viewport(std::int32_t x, std::int32_t y, std::int32_t width, std::int32_t height);
	Viewport(std::int32_t x, std::int32_t y, std::int32_t width, std::int32_t height, float minDepth, float maxDepth);

	///@~Japanese
	/// @brief 矩形の水平方向の長さを設定します。
	void SetWidth(std::int32_t width);
	
	///@~Japanese
	/// @brief 矩形の垂直方向の高さを設定します。
	void SetHeight(std::int32_t height);

	///@~Japanese
	/// @brief 矩形の水平方向の長さを計算します。
	std::int32_t GetWidth() const;
	
	///@~Japanese
	/// @brief 矩形の垂直方向の高さを計算します。
	std::int32_t GetHeight() const;
	
	///@~Japanese
	/// @brief 矩形の左端の X 座標を計算します。
	std::int32_t TopLeftX() const;
	
	///@~Japanese
	/// @brief 矩形の左端の Y 座標を計算します。
	std::int32_t TopLeftY() const;

	///@~Japanese
	/// @brief アスペクト比を計算します。
	float AspectRatio() const;
};

/// @}
/// @}

}// namespace Pomdog

#endif // !defined(POMDOG_VIEWPORT_AA637108_9222_46B7_BC9D_293048246897_HPP)
