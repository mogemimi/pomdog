//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_SPRITE_3EEA26CB_0AA7_4E99_9D42_A28F19EFD4FC_HPP
#define POMDOG_SPRITE_3EEA26CB_0AA7_4E99_9D42_A28F19EFD4FC_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <Pomdog/Config/Export.hpp>
#include <Pomdog/Math/Rectangle.hpp>
#include <Pomdog/Math/Vector2.hpp>

namespace Pomdog {

/// @addtogroup Framework
/// @{
/// @addtogroup Gameplay2D
/// @{

class POMDOG_EXPORT Sprite {
public:
	Rectangle Subrect;
	Vector2 Origin;
	//float LayerDepth; // Draw Order
	
	Sprite()
		: Subrect(0, 0, 1.0f, 1.0f)
		, Origin(0.5f, 0.5f)
	{}
};

/// @}
/// @}

}// namespace Pomdog

#endif // !defined(POMDOG_SPRITE_3EEA26CB_0AA7_4E99_9D42_A28F19EFD4FC_HPP)
