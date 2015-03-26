// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_TEXTUREREGION_9F020F68_HPP
#define POMDOG_TEXTUREREGION_9F020F68_HPP

#include "Pomdog/Math/Rectangle.hpp"
#include <cstdint>

namespace Pomdog {

class TextureRegion {
public:
    Rectangle Subrect;
    std::uint16_t XOffset;
    std::uint16_t YOffset;
    std::uint16_t Width;
    std::uint16_t Height;
    bool Rotate;
};

}// namespace Pomdog

#endif // POMDOG_TEXTUREREGION_9F020F68_HPP
