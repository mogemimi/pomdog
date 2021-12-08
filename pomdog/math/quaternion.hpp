// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/math/detail/floating_point_quaternion.hpp"

namespace pomdog {

/// Quaternion represents a rotation around an axis in 3D space.
using Quaternion = detail::FloatingPointQuaternion<float>;

} // namespace pomdog
