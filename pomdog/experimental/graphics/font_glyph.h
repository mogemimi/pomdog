// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/types.h"
#include "pomdog/math/rect2d.h"

namespace pomdog {

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
    Rect2D subrect;

    // NOTE: UCS-4/UTF-32
    char32_t character;

    i16 xOffset;
    i16 yOffset;
    i16 xAdvance;
    i16 texturePage;
};

} // namespace pomdog
