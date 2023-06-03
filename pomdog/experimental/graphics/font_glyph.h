// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/math/rectangle.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstdint>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

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
    Rectangle subrect;

    // NOTE: UCS-4/UTF-32
    char32_t character;

    std::int16_t xOffset;
    std::int16_t yOffset;
    std::int16_t xAdvance;
    std::int16_t texturePage;
};

} // namespace pomdog
