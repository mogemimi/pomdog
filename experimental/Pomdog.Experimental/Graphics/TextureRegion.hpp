// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Math/Rectangle.hpp"
#include <cstdint>

namespace Pomdog {

class TextureRegion {
public:
    Rectangle Subrect;
    std::int16_t XOffset;
    std::int16_t YOffset;
    std::int16_t Width;
    std::int16_t Height;
    bool Rotate;
};

} // namespace Pomdog
