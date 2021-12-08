// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/math/vector3.hpp"

namespace pomdog {

class AudioEmitter final {
public:
    Vector3 Forward;
    Vector3 Position;
    Vector3 Up;
    Vector3 Velocity;
    float DopplerScale;
};

} // namespace pomdog
