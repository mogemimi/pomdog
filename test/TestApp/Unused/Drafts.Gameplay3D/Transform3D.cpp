//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#include "Transform3D.hpp"

namespace Pomdog {

Transform3D::Transform3D()
	: Position(Vector3::Zero)
	, Rotation(Vector3::Zero)
	, Scale(1.0f, 1.0f, 1.0f)
{}

}// namespace Pomdog
