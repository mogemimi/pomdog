// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/math/detail/floating_point_quaternion.hpp"

namespace Pomdog {

/// Quaternion represents a rotation around an axis in 3D space.
using Quaternion = Detail::FloatingPointQuaternion<float>;

} // namespace Pomdog
