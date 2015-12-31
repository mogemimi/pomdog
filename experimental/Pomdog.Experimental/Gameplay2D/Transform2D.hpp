// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog.Experimental/Gameplay/Component.hpp"
#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Math/Vector2.hpp"
#include "Pomdog/Math/Radian.hpp"

namespace Pomdog {

class POMDOG_EXPORT Transform2D: public Component<Transform2D> {
public:
    Vector2 Position {0, 0};
    Vector2 Scale {1, 1};
    Radian<float> Rotation {0};
};

} // namespace Pomdog
