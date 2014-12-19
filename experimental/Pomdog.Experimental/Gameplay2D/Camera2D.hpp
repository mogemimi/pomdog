//
//  Copyright (C) 2013-2014 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_CAMERA2D_6E6D82E9_5A8E_4A79_9B34_2A6BE225A7FF_HPP
#define POMDOG_CAMERA2D_6E6D82E9_5A8E_4A79_9B34_2A6BE225A7FF_HPP

#if (_MSC_VER > 1000)
#pragma once
#endif

#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Gameplay/Component.hpp"
#include <cstdint>

namespace Pomdog {

class POMDOG_EXPORT Camera2D: public Component<Camera2D> {
public:
	// range: [std::numeric_limits<float>::epsilon(), max()] or (0, max()]
	float Zoom = 1;
	
	// Clipping Planes
	float Near = 0.1f;
	float Far = 1000.0f;
	
	float NormalizedViewportX = 0.0f;
	float NormalizedViewportY = 0.0f;
	float NormalizedViewportWidth = 1.0f;
	float NormalizedViewportHeight = 1.0f;
};

}// namespace Pomdog

#endif // !defined(POMDOG_CAMERA2D_6E6D82E9_5A8E_4A79_9B34_2A6BE225A7FF_HPP)
