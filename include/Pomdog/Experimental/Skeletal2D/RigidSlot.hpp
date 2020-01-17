// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Experimental/Skeletal2D/JointIndex.hpp"
#include "Pomdog/Math/Color.hpp"
#include "Pomdog/Math/Radian.hpp"
#include "Pomdog/Math/Rectangle.hpp"
#include "Pomdog/Math/Vector2.hpp"
#include <cstdint>

namespace Pomdog::Skeletal2D {

class RigidSlot final {
public:
    Rectangle Subrect;
    Vector2 Translate;
    Vector2 Scale;
    Radian<float> Rotation;
    Pomdog::Color Color;
    Vector2 Origin;
    Skeletal2D::JointIndex JointIndex;
    std::uint32_t HashID;
    std::int16_t DrawOrder;
    std::int16_t TexturePage;
    bool TextureRotate;
};

} // namespace Pomdog::Skeletal2D
