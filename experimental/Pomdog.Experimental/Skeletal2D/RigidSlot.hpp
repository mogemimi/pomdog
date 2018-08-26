// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "JointIndex.hpp"
#include "Pomdog/Math/Rectangle.hpp"
#include "Pomdog/Math/Vector2.hpp"
#include "Pomdog/Math/Color.hpp"
#include "Pomdog/Math/Radian.hpp"
#include <cstdint>

namespace Pomdog {

class RigidSlot {
public:
    Rectangle Subrect;
    Vector2 Translate;
    Vector2 Scale;
    Radian<float> Rotation;
    Pomdog::Color Color;
    Vector2 Origin;
    Pomdog::JointIndex JointIndex;
    std::uint32_t HashId;
    std::int16_t DrawOrder;
    std::int16_t TexturePage;
    bool TextureRotate;
};

} // namespace Pomdog
