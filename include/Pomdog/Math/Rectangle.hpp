//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_RECTANGLE_4B874E7C_F063_45FC_9ED7_B493476A646C_HPP
#define POMDOG_RECTANGLE_4B874E7C_F063_45FC_9ED7_B493476A646C_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include "../Config/Export.hpp"
#include "../Config/FundamentalTypes.hpp"
#include "Point2D.hpp"

namespace Pomdog {

/// @addtogroup Framework
/// @{
/// @addtogroup Math
/// @{

///@~Japanese
/// @brief 長方形を表す幾何学クラスです。
/// @code
/// // Image:
/// //           width			
/// //  _______________________					  
/// // |* <--(x, y)            |	
/// // |                       |	
/// // |                       |				
/// // |           *           | height			
/// // |         Center        |	
/// // |                       | Right = x + width
/// // |                       |	
/// // |_______________________|	
/// // 								
/// //    Bottom = y + height		
/// //
/// @endcode
class POMDOG_EXPORT Rectangle
{
public:
	std::int32_t x, y;
	std::int32_t width, height;

public:
	// Constructors:
	Rectangle() = default;
	Rectangle(Rectangle const&) = default;
	Rectangle(Rectangle &&) = default;
	
	Rectangle(std::int32_t x, std::int32_t y, std::int32_t width, std::int32_t height);
	Rectangle(Point2D const& position, std::int32_t width, std::int32_t height);

	~Rectangle() = default;

	// Assignment operators:
	Rectangle & operator=(Rectangle const&) = default;
	Rectangle & operator=(Rectangle &&) = default;

	// Binary operators:
	bool operator==(Rectangle const&) const;
	bool operator!=(Rectangle const&) const;

	///@~Japanese
	/// @brief 矩形の下端の Y 座標を計算します。
	std::int32_t Bottom() const;

	///@~Japanese
	/// @brief 矩形の右端の X 座標を計算します。
	std::int32_t Right() const;

	///@~Japanese
	/// @brief 矩形の上端の Y 座標を計算します。
	std::int32_t Top() const;

	///@~Japanese
	/// @brief 矩形の左端の X 座標を計算します。
	std::int32_t Left() const;

	///@~Japanese
	/// @brief 矩形の中心点の座標を計算します。
	Point2D Center() const;

	///@~Japanese
	/// @brief 矩形の左上隅の位置を取得します。
	Point2D GetLocation() const;

	///@~Japanese
	/// @brief 矩形の左上隅の位置を設定します。
	void SetLocation(Point2D const& position);

	///@~Japanese
	/// @brief 矩形の位置をオフセット値で変更します。
	void Offset(std::int32_t offsetX, std::int32_t offsetY);

	///@~Japanese
	/// @brief 矩形の位置をオフセット値で変更します。
	void Offset(Point2D const& offset);
};

/// @}
/// @}

}// namespace Pomdog

#endif // !defined(POMDOG_RECTANGLE_4B874E7C_F063_45FC_9ED7_B493476A646C_HPP)
