//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "Transform3D.hpp"

namespace Pomdog {

Transform3D::Transform3D()
	: Position(Vector3::Zero)
	, Rotation(Vector3::Zero)
	, Scale(1.0f, 1.0f, 1.0f)
{}

}// namespace Pomdog
