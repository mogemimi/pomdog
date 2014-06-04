//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include <Pomdog/Gameplay2D/Camera2D.hpp>

namespace Pomdog {
//-----------------------------------------------------------------------
void Camera2D::Zoom(float value)
{
	this->zoom = std::max(value, std::numeric_limits<float>::epsilon());
}
//-----------------------------------------------------------------------
float Camera2D::Zoom() const
{
	return this->zoom;
}

}// namespace Pomdog
