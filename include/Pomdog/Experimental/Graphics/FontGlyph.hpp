// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Math/Rectangle.hpp"
#include <cstdint>

namespace Pomdog {

// NOTE:
//          XAdvance
//  <----------------------->
//
//  +-----------------------+
//  |         ^             |
//  |         |             |
//  |         | YOffset     |
//  |         v             |
//  |<------->+-----------+ |
//  | XOffset |           | |
//  |         |  Subrect  | |
//  |         |           | |
//  |         +-----------+ |
//  +-----------------------+

struct FontGlyph final {
    Rectangle Subrect;

    // NOTE: UCS-4/UTF-32
    char32_t Character;

    std::int16_t XOffset;
    std::int16_t YOffset;
    std::int16_t XAdvance;
    std::int16_t TexturePage;
};

} // namespace Pomdog
