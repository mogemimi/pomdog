// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_RIGIDSLOT_4C507F41_HPP
#define POMDOG_RIGIDSLOT_4C507F41_HPP

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
    Color Color;
    Vector2 Origin;
    JointIndex JointIndex;
    std::uint32_t HashId;
    std::int16_t DrawOrder;
    std::int16_t TexturePage;
    bool TextureRotate;
};

} // namespace Pomdog

#endif // POMDOG_RIGIDSLOT_4C507F41_HPP
