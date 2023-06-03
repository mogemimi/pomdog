// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/math/vector2.h"

namespace pomdog {

struct GamepadThumbSticks final {
    Vector2 left = Vector2::createZero();
    Vector2 right = Vector2::createZero();
};

} // namespace pomdog
