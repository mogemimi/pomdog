//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_CAMERA2D_6E6D82E9_5A8E_4A79_9B34_2A6BE225A7FF_HPP
#define POMDOG_CAMERA2D_6E6D82E9_5A8E_4A79_9B34_2A6BE225A7FF_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <Pomdog/Config/Export.hpp>
#include <algorithm>
#include <limits>

namespace Pomdog {

/// @addtogroup Framework
/// @{
/// @addtogroup Gameplay2D
/// @{

class POMDOG_EXPORT Camera2D {
public:
	void Zoom(float value);
	
	float Zoom() const;
	
private:
	float zoom = 1;
};

/// @}
/// @}

}// namespace Pomdog

#endif // !defined(POMDOG_CAMERA2D_6E6D82E9_5A8E_4A79_9B34_2A6BE225A7FF_HPP)
