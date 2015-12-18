// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#ifndef POMDOG_TRANSFORM2D_9973161D_HPP
#define POMDOG_TRANSFORM2D_9973161D_HPP

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

#endif // POMDOG_TRANSFORM2D_9973161D_HPP
