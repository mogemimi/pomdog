// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/math/vector3.h"

namespace pomdog {

class AudioEmitter final {
public:
    Vector3 forward;
    Vector3 position;
    Vector3 up;
    Vector3 velocity;
    float dopplerScale;
};

} // namespace pomdog
