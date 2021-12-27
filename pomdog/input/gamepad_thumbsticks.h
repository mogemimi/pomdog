// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/math/vector2.h"

namespace pomdog {

struct GamepadThumbSticks final {
    Vector2 Left = Vector2::Zero();
    Vector2 Right = Vector2::Zero();
};

} // namespace pomdog
