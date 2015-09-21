// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_JOINTPOSE_160B1EF6_HPP
#define POMDOG_JOINTPOSE_160B1EF6_HPP

#include "Pomdog/Math/Vector2.hpp"
#include "Pomdog/Math/Radian.hpp"

namespace Pomdog {

class JointPose {
public:
    Vector2 Translate;
    Radian<float> Rotation;
    float Scale;
};

} // namespace Pomdog

#endif // POMDOG_JOINTPOSE_160B1EF6_HPP
