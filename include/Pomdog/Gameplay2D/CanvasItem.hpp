//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_CANVASITEM_2E0BA3CA_4A40_4E8F_A355_E153025587B9_HPP
#define POMDOG_CANVASITEM_2E0BA3CA_4A40_4E8F_A355_E153025587B9_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <Pomdog/Config/Export.hpp>

namespace Pomdog {

/// @addtogroup Framework
/// @{
/// @addtogroup Gameplay2D
/// @{

class POMDOG_EXPORT CanvasItem
{
public:
	bool Visibile;
	
	CanvasItem()
		: Visibile(true)
	{}
};

/// @}
/// @}

}// namespace Pomdog

#endif // !defined(POMDOG_CANVASITEM_2E0BA3CA_4A40_4E8F_A355_E153025587B9_HPP)
